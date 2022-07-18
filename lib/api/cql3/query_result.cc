#include "api/cql3/query_result.hh"
#include "api/cql3/server_response.hh"
#include "catalog/data.hh"
#include "catalog/schema.hh"
#include <memory>

using namespace dibibase::api::cql3;
using namespace dibibase::catalog;
int QueryResult::schema_change(std::string table){
  body[0] = 0xFF;body[1]=0xFF;
  body[2] = ServerMsg::Opcode::EVENT;
  int index = 7;
  std::string sch_change = "SCHEMA_CHANGE";
  std::string created = "CREATED";
  std::string table_ = "TABLE";
  index = count(body, index, 2, sch_change.length());
  index = append_string(body, index, sch_change.length(), sch_change);
  index = count(body, index, 2, created.length());
  index = append_string(body, index, created.length(), created);
  index = count(body, index, 2,table_.length());
  index = append_string(body, index, table_.length(), table_);

  index = count(body, index, 2, keyspace.size());
  index = append_string(body, index, keyspace.size(), keyspace);
  index = count(body, index, 2, table.size());
  index = append_string(body, index, table.size(), table);
  count(body, 3, 4, index-6);
  return index;
}
int QueryResult::create_table(std::string table){
  //0,0,0,5,
  body[0] = 0;body[1] = 0;body[2]=0;body[3]=5; //Schema_change
  int index = 4;

  std::string created = "CREATED";
  std::string table_ = "TABLE";
  index = count(body, index, 2, created.length());
  index = append_string(body, index, created.length(), created);
  index = count(body, index, 2,table_.length());
  index = append_string(body, index, table_.length(), table_);

  index = count(body, index, 2, keyspace.size());
  index = append_string(body, index, keyspace.size(), keyspace);
  index = count(body, index, 2, table.size());
  index = append_string(body, index, table.size(), table);

  return index;
}
int QueryResult::insert_record(){
  body[0] = 0;body[1]=0;body[2]=0;body[3]=4;    //prepared
  body[4] = 0;body[5]=0;body[6]=0;body[7]=1;
  return 8;
}
int QueryResult::select_result(std::string table, catalog::Schema schema,
                               std::vector<catalog::Record> record_v,std::string record_str) {
  int rows = record_v.size();


  if (rows < 1 && record_str == "") {
    body[0] = 0;
    body[1] = 0;
    body[2] = 0;
    body[3] = 1;
    // Void = 0,0,0,1
  } else {
    // 0,0,0,2 (rows)
    body[0] = 0;
    body[1] = 0;
    body[2] = 0;
    body[3] = 2;
  }
  if (rows <= 10 && rows >= 0)
    body[4] = 0, body[5] = 0, body[6] = 0, body[7] = 1; // global_tables_spec
  else if (rows > 10)
    body[4] = 0, body[5] = 0, body[6] = 0, body[7] = 2; // Has_more_pages
  size = 8;
  int index = 7;
  /*
  / Appending Kespace & table strings
  */
  index = count(body, 8, 4, schema.fields().size());
  index = count(body, index, 2, keyspace.size());
  index = append_string(body, index, keyspace.size(), keyspace);
  index = count(body, index, 2, table.size());
  index = append_string(body, index, table.size(), table);

  // Schema serialization //
  for (int i = 0; i < int(schema.fields().size()); i++) {

    catalog::Field f = schema.fields()[i];
    const catalog::Data::Type t = f.type();
    index = count(body, index, 2, f.name().length());
    index = append_field(body, index, f.name().length(), f.name(), t);
  }
  // records serialization //
  index = count(body, index, 4, rows+1);

  std::string delimiter = "|"; //between each value inside one record
  std::vector<std::string> values;
  values = split(record_str,delimiter);
  for (auto val : values){
      if (val.length()>0){
      delimiter = ",";
      std::vector<std::string> attributes;
      attributes = split(val,delimiter);
      index = count(body, index, 4, stoi(attributes[2]));
      index = append_record(body, index, stoi(attributes[2]), attributes[0],
                            stoi(attributes[1]));

      }
  }      
      



  //Depricated (local installation)
  /*
  for (auto &record : record_v) {
    std::vector<std::shared_ptr<catalog::Data>> d = record.values();
    for (auto &data : d) {
      index = count(body, index, 4, data->type().length());
      util::Buffer *buf = new util::MemoryBuffer(50);
      data->bytes(buf);
      index = append_record(body, index, data->type().length(), data->print(),
                            data->type().id());
    }
  }
  */
  index--;
  size = index + 1;
  return size;
}
int QueryResult::count(char *body, int start, int breadth, int size) {
  if (size < 256 * 256 * 256 && size > 256 * 256) {
    for (int i = start; i < start + breadth; i++) {
      body[i] = 0;
      if (i - start == breadth - 3)
        body[i] = int(size/65536);
      else if (i-start == breadth - 2)
        body[i] = int((size - body[i-1]*65536)/256);
      else if (i-start == breadth - 1)
        body[i] = size - body[i-1]*256 - body[i-2]*65536;
    }
  } else if (size < 256 * 256 && size > 256) {
    for (int i = start; i < start + breadth; i++) {
      body[i] = 0;
      if (i - start == breadth - 2)
        body[i] = int(size/256);
      else if (i-start == breadth - 1)
        body[i] = size - body[i-1]*256;
    }
  } else if (size < 256) {
    for (int i = start; i < start + breadth; i++) {
      body[i] = 0;
      if (i - start == breadth - 1)
        body[i] = size;
    }
  }
  return start + breadth;
}

int QueryResult::append_string(char *buffer, int start, int size,
                               std::string input) {
  int in_index = 0;
  for (int i = start; i < start + size; i++) {
    buffer[i] = (int)input[in_index];
    in_index++;
  }
  return start + size;
}
int QueryResult::append_field(char *buffer, int start, int size,
                              std::string field_name, catalog::Data::Type t) {
  int in_index = 0;
  buffer[size + start] = 0;
  for (int i = start; i < size + start; i++) {
    buffer[i] = (int)field_name[in_index];
    in_index++;
  }
  int type_index = size + start + 1;
  // mapping our data types to cql basic types //
  switch (t.id()) {
  case Data::Type::Id::ASCII: {
    buffer[type_index] = Ascii;
    break;
  }
  case Data::Type::Id::BIGINT: {
    buffer[type_index] = Bigint;
    break;
  }
  case Data::Type::Id::BOOLEAN: {
    buffer[type_index] = Boolean;
    break;
  }
  case Data::Type::Id::DOUBLE: {
    buffer[type_index] = Double;
    break;
  }
  case Data::Type::Id::FLOAT: {
    buffer[type_index] = Float;
    break;
  }
  case Data::Type::Id::INT: {
    buffer[type_index] = Int;
    break;
  }
  case Data::Type::Id::SMALLINT: {
    buffer[type_index] = Smallint;
    break;
  }
  case Data::Type::Id::TINYINT: {
    buffer[type_index] = Tinyint;
    break;
  }
  case Data::Type::Id::BLOB:
    buffer[type_index] = Blob;
    break;
  }
  return start + size + 2;
}

int QueryResult::append_record(char *buffer, int start, int size,
                               std::string record_str, int id) {

  switch (id) {
  case Data::Type::Id::ASCII: {
    int in_index = 0;
    for (int i = start; i < size + start; i++) {
      buffer[i] = (int)record_str[in_index];
      in_index++;
    }
    break;
  }
  case Data::Type::Id::BIGINT: {

    break;
  }
  case Data::Type::Id::BOOLEAN: {

    break;
  }
  case Data::Type::Id::DOUBLE: {

    break;
  }
  case Data::Type::Id::FLOAT: {

    break;
  }
  case Data::Type::Id::INT: {
    int int_data = stoi(record_str);
    count(body, start, 4, int_data);
    break;
  }
  case Data::Type::Id::SMALLINT: {

    break;
  }
  case Data::Type::Id::TINYINT: {

    break;
  }
  case Data::Type::Id::BLOB:
    break;
  }

  return start + size;
}

std::vector<std::string> QueryResult::split(std::string str, std::string token){
    std::vector<std::string> result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}