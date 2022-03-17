#include "api/cql3/query_result.hh"
#include "catalog/data.hh"
#include "catalog/schema.hh"
#include <memory>

using namespace dibibase::api::cql3;
using namespace dibibase::catalog;

int QueryResult::select_result(std::string table, catalog::Schema schema,
                               std::vector<catalog::Record> record_v) {
  int rows = record_v.size();
  if (rows < 1) {
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
  if (rows < 10)
    body[4] = 0, body[5] = 0, body[6] = 0, body[7] = 1; // global_tables_spec
  else
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
    index = count(body, index, 2, t.size());
    index = append_field(body, index, t.size(), f.name(), t);
  }
  // records serializatio //
  index = count(body, index, 4, rows);

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

  index--;
  size = index + 1;
  return size;
}
int QueryResult::count(char *body, int start, int breadth, int size) {
  if (size < 256 * 256 * 256 && size > 256 * 256) {

  } else if (size < 256 * 256 && size > 256) {

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
                               std::string record_str, Data::Type::Id id) {

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