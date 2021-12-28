#include "catalog/record.hh"
#include <memory>
#include <iostream>

using namespace dibibase::util;
using namespace dibibase::catalog;

  Record::from(util::Buffer *buffer, Schema schema){
    std::vector<Data> data;
    std::unique_ptr<Data> data_ptr;
    std::vector<Field> fields = schema.fields();
    using iter = std::vector<Field>::const_iterator;
  
      for(iter it2= fields.begin() ; it2 != fields.end(); ++it2){
            //int size = it2->size();
            Data::Type type = it2->type();

            if (type.id() == Data::Type::ASCII){
                std::unique_ptr<Data> data_ptr = Data::from(buffer,type);
                data.push_back(*data_ptr);
            }
      }
    return data;
  }


  /**
  *  @brief Input is a list of Data and output is ptr to arr of bytes
  */
    Record::bytes(util::Buffer *buffer) {
    std::unique_ptr<char[]> bytes;
    using Iter = std::vector<Data>::const_iterator;
    Data *data = nullptr;
    for (Iter it = m_values.begin(); it != m_values.end(); ++it) {
      // check the data type
      Data::Type type = it->type();

      switch (type.id()) {
      case Data::Type::ASCII: {
        ASCIIData *ascii_data = dynamic_cast<ASCIIData *>(data);
        string s = ascii_data->data();
        buffer->put_string(s);
        break;
      }
      case Data::Type::BIGINT: {
        BigIntData *bigint_data = dynamic_cast<BigIntData *>(data);
        uint64_t bigint_d = bigint_data->data();
        buffer->put_uint64(bigint_d);
        break;
      }
      case Data::Type::BOOLEAN: {
        break;
      }
      case Data::Type::DOUBLE: {
        break;
      }
      case Data::Type::FLOAT: {
        break;
      }
      case Data::Type::INT: {
        break;
      }
      case Data::Type::SMALLINT: {
        break;
      }
      case Data::Type::TINYINT: {
        break;
      }
      case Data::Type::BLOB:{
          break;
      }
      }
    }

    return bytes;
  }