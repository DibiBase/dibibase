#include "api/cql3/server_response.hh"

#include "Exceptions.h"
#include "InputMismatchException.h"
#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"
#include <chrono>
#include "api/cql3/fake_meta.hh"
#include "api/cql3/query_result.hh"
#include "db/table_manager.hh"
#include "dht/query_processor.hh"
#include "lang/statements/create_table_statement.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"
#include <exception>
#include <tuple>

using namespace antlr4;
using namespace dibibase;
using namespace dibibase::lang::cql3;

using namespace dibibase::api::cql3;
using namespace std::chrono;
ServerMsg::ServerMsg(Frame f) { frame = f; }
int ServerMsg::SupportedMessage(
    std::map<std::string, std::list<std::string>> supportedOptions,
    unsigned char *Header) {
  int index = 9;
  for (auto it = supportedOptions.begin(); it != supportedOptions.end(); ++it) {
    std::string s = it->first;
    if (it == supportedOptions.begin()) {
      Header[index] = 0;
      Header[++index] = supportedOptions.size();
      index++;
    }
    Header[index] = 0;
    Header[++index] = s.length();
    index++;
    for (unsigned int i = 0; i < s.length(); i++) {
      Header[index] = (int)s[i];
      index++;
    }

    for (std::list<std::string>::iterator it2 = (it->second).begin();
         it2 != (it->second).end(); ++it2) {
      if (it2 == (it->second).begin()) {
        Header[index] = 0;
        Header[++index] = it->second.size();
        index++;
      }
      Header[index] = 0;
      Header[++index] = (*it2).length();
      index++;

      std::string list_elem = *it2;
      for (unsigned int k = 0; k < list_elem.length(); k++) {
        Header[index] = (int)list_elem[k];
        index++;
      }
    }
  }
  Header[8] = (int)(index - 9); // count number of bytes (should be < 255
                                // otherwise use Header[7]&Header[6])
  return index;
}

int ServerMsg::CreateResponse(std::shared_ptr<dibibase::db::Database> db,int met[]) {
  int opcode = int(frame.opcode);
  int msg_length = 9; // default minimum is 9 decimals (HEADER ONLY NO DATA)
  int responce_version = frame.version + 0x80;
  std::list<std::string> versions, comp_types, cql_versions;
  Header[0] = responce_version;
  Header[1] = frame.flags;
  Header[2] = frame.streamID[0];
  Header[3] = frame.streamID[1];
  switch (opcode) {
  case OPTIONS: {
    Header[4] = SUPPORTED;
    Header[5] = 0;
    Header[6] = 0;
    Header[7] = 0;
    versions.push_back("3/V3");
    comp_types.push_back("snappy");
    comp_types.push_back("lz4");
    cql_versions.push_back("3.4.5");

    std::map<std::string, std::list<std::string>> supportedOptions;
    supportedOptions.insert(make_pair("PROTOCOL_VERSIONS", versions));
    supportedOptions.insert(make_pair("CQL_VERSION", cql_versions));
    supportedOptions.insert(make_pair("COMPRESSION", comp_types));
    msg_length = SupportedMessage(supportedOptions, Header);
    break;
  }
  case STARTUP: {
    Header[4] = READY;
    Header[5] = 0;
    Header[6] = 0;
    Header[7] = 0;
    Header[8] = 0;
    msg_length = 9;

    break;
  }
  case REGISTER: {
    Header[4] = READY;
    Header[5] = 0;
    Header[6] = 0;
    Header[7] = 0;
    Header[8] = 0;
    msg_length = 9;

    break;
  }
  case QUERY: {
    Header[4] = RESULT;

    int escape_flag = 0;
    int meta = 1;

    for (int i = 0; i < frame.max_size - 9; i++) {
      if (frame.body[i] <= 0)
        escape_flag = 1;
      else if (escape_flag == 1 && frame.body[i] != 0)
        escape_flag = 0;
      else if (escape_flag == 0)
        query += frame.body[i];
    }
    if (query != "" && strstr(query.c_str(), ";")) {
      meta = 0; // non metadata query (normal query)
      if (query.back() != ';')
        query = query.substr(0, query.find(';') + 1);
    }
    if (meta) {
      unsigned char body[30000];
      Metadata m(body);
      msg_length = m.meta_result(query);

      for (int i = 5; i < msg_length; i++)
        Header[i] = body[i - 5];

    }

    else if (query != "" && query.back() == ';') {
      // Handling sliced user queries (in a bad way)
      if (tolower(query[0]) == 'n')
        query = "I" + query;
      else if (tolower(query[0]) == 'r')
        query = "C" + query;
      else if (tolower(query[0]) == 'e'&&tolower(query[4]) == 'c')
        query = "S" + query;
      else if (tolower(query[0]) == 'e'&&tolower(query[4])=='t')
        query = "D" + query;

      char body[3500];
      try{
      ANTLRInputStream input(query);
      CqlLexer lexer(&input);
      CommonTokenStream tokens(&lexer);
      CqlParser parser(&tokens);
      CqlParser::RootContext *tree = parser.root();
      CQL3Visitor visitor;
      std::vector<lang::Statement *> statements =
          visitor.visitRoot(tree).as<std::vector<lang::Statement *>>();
     
      dht::QueryProcessor qp(query);
      string record_str_result = qp.process();
      
      for (auto statement : statements) {
        switch (statement->type()) {
          
        case lang::Statement::Type::CREATE_TABLE:
        try{
          if (lang::CreateTableStatement *create_table_statement =
                  dynamic_cast<lang::CreateTableStatement *>(statement);
              create_table_statement != nullptr) {
            //create_table_statement->execute(*db);
            met[0] += 1;
            int stream_id = Header[3];
            QueryResult q(create_table_statement->m_keyspace, body);
            int size1 = q.schema_change(create_table_statement->m_table);
            for (int i = 2; i < size1 + 2; i++)
              Header[i] = body[i - 2];

            Header[size1 + 2] = 0;
            Header[size1 + 3] = 1;
            Header[size1 + 4] = 0;
            Header[size1 + 2] = responce_version;
            Header[size1 + 3] = 0;
            Header[size1 + 4] = 0;
            Header[size1 + 5] = stream_id;
            Header[size1 + 6] = 0x8;
            int index = size1 + 7;
            int size2 = q.create_table(create_table_statement->m_table);
            Header[index] = 0;
            Header[++index] = 0;
            Header[++index] = 0;
            Header[++index] = size2;
            index++;
            for (int i = index; i < size2 + index; i++)
              Header[i] = body[i - index];

            msg_length = size1 + 2 + size2 + 12;
          }
        }
        catch(unspported_cql_statement){met[7]++;}
          break;
        case lang::Statement::Type::INSERT:
        try{
          if (lang::InsertStatement *insert_statement =
                  dynamic_cast<lang::InsertStatement *>(statement);
              insert_statement != nullptr) {
            auto start = high_resolution_clock::now();
            //insert_statement->execute(*db);

            QueryResult q(insert_statement->m_keyspace, body);
            int size = q.insert_record();
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            met[3] = duration.count();
            met[1] += 1;
            for (int i = 5; i < size + 5; i++)
              Header[i] = body[i - 5];
            msg_length = size + 5;
          }
        }
        catch(dibibase::db::schema_mismatch_error){met[7]++;}
          break;
        case lang::Statement::Type::SELECT:
          if (lang::SelectStatement *select_statement =
                  dynamic_cast<lang::SelectStatement *>(statement);
              select_statement != nullptr) {
            auto start = high_resolution_clock::now();
            //auto result = select_statement->execute(*db);  (Deprecated)
            std::vector<catalog::Record> record_v; 
            //record_v.push_back(result.value());   (Deprecated)
            
            const std::unique_ptr<catalog::Schema> &s =
                db->read_schema(select_statement->m_from_spec.m_table);
            QueryResult q(select_statement->m_from_spec.m_keyspace, body);
            int size = q.select_result(select_statement->m_from_spec.m_table,
                                       *s, record_v, record_str_result);
            
            auto stop = high_resolution_clock::now();

            auto duration = duration_cast<microseconds>(stop - start);
            met[4] = duration.count();
            met[2] += 1;
            Header[5] = 0;
            Header[6] = 0;
            Header[7] = 0;
            Header[8] = size;
            for (int i = 9; i < size + 9; i++)
              Header[i] = body[i - 9];

            msg_length = size + 9;
          }
          break;
        default:
          break;
        }

        delete statement;
      }

    } //
     catch(dibibase::db::non_existent_record_error){
        util::Logger::make().err("code=2200 [Invalid query] message=\"record couldn't be found\"");
       met[7]++;} 
     catch(unspported_cql_statement){met[7]++;}
    }
    break;
  }
  case AUTHENTICATE:

    break;
  }
  return msg_length;
}

std::string ServerMsg::get_query() { return query; }
