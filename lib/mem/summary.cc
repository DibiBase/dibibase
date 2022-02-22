#include "mem/summary.hh"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>

using namespace dibibase::mem;
using namespace dibibase::catalog;

std::unique_ptr<Summary> Summary::from(util::Buffer *buff, Data::Type type) {
  std::unique_ptr<Summary> summary = std::make_unique<Summary>();

  int8_t total_pages = buff->get_int8();

  while (summary->size() < total_pages) {
    std::unique_ptr<Data> data = Data::from(buff, type);
    summary->push_back(data.release());
  }

  return summary;
}

off_t Summary::find_index_page(catalog::Data *key) {
  // CHECK: Maybe the logic is wrong. (perform binary search instead)
  for (size_t i = 0; i < m_sort_keys.size() - 1; i++) {
    // Extracting data type, and getting its id.
    Data::Type::Id key_id = m_sort_keys[i]->type().id();

    // TODO: Find a better way !!
    switch (key_id) {
    case Data::Type::ASCII: {
      ASCIIData *current_key = dynamic_cast<ASCIIData *>(m_sort_keys[i]);
      ASCIIData *next_key = dynamic_cast<ASCIIData *>(m_sort_keys[i + 1]);
      ASCIIData *given_key = dynamic_cast<ASCIIData *>(key);

      std::string c_key = current_key->data();
      std::string n_key = next_key->data();
      std::string g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::BIGINT: {
      BigIntData *current_key = dynamic_cast<BigIntData *>(m_sort_keys[i]);
      BigIntData *next_key = dynamic_cast<BigIntData *>(m_sort_keys[i + 1]);
      BigIntData *given_key = dynamic_cast<BigIntData *>(key);

      int64_t c_key = current_key->data();
      int64_t n_key = next_key->data();
      int64_t g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::BOOLEAN: {
      BooleanData *current_key = dynamic_cast<BooleanData *>(m_sort_keys[i]);
      BooleanData *next_key = dynamic_cast<BooleanData *>(m_sort_keys[i + 1]);
      BooleanData *given_key = dynamic_cast<BooleanData *>(key);

      bool c_key = current_key->data();
      bool n_key = next_key->data();
      bool g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::DOUBLE: {
      DoubleData *current_key = dynamic_cast<DoubleData *>(m_sort_keys[i]);
      DoubleData *next_key = dynamic_cast<DoubleData *>(m_sort_keys[i + 1]);
      DoubleData *given_key = dynamic_cast<DoubleData *>(key);

      double c_key = current_key->data();
      double n_key = next_key->data();
      double g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::FLOAT: {
      FloatData *current_key = dynamic_cast<FloatData *>(m_sort_keys[i]);
      FloatData *next_key = dynamic_cast<FloatData *>(m_sort_keys[i + 1]);
      FloatData *given_key = dynamic_cast<FloatData *>(key);

      float c_key = current_key->data();
      float n_key = next_key->data();
      float g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::INT: {
      IntData *current_key = dynamic_cast<IntData *>(m_sort_keys[i]);
      IntData *next_key = dynamic_cast<IntData *>(m_sort_keys[i + 1]);
      IntData *given_key = dynamic_cast<IntData *>(key);

      int32_t c_key = current_key->data();
      int32_t n_key = next_key->data();
      int32_t g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::SMALLINT: {
      SmallIntData *current_key = dynamic_cast<SmallIntData *>(m_sort_keys[i]);
      SmallIntData *next_key = dynamic_cast<SmallIntData *>(m_sort_keys[i + 1]);
      SmallIntData *given_key = dynamic_cast<SmallIntData *>(key);

      int16_t c_key = current_key->data();
      int16_t n_key = next_key->data();
      int16_t g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::TINYINT: {
      TinyIntData *current_key = dynamic_cast<TinyIntData *>(m_sort_keys[i]);
      TinyIntData *next_key = dynamic_cast<TinyIntData *>(m_sort_keys[i + 1]);
      TinyIntData *given_key = dynamic_cast<TinyIntData *>(key);

      int8_t c_key = current_key->data();
      int8_t n_key = next_key->data();
      int8_t g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    case Data::Type::BLOB: {
      BlobData *current_key = dynamic_cast<BlobData *>(m_sort_keys[i]);
      BlobData *next_key = dynamic_cast<BlobData *>(m_sort_keys[i + 1]);
      BlobData *given_key = dynamic_cast<BlobData *>(key);

      std::unique_ptr<unsigned char[]> c_key = current_key->data();
      std::unique_ptr<unsigned char[]> n_key = next_key->data();
      std::unique_ptr<unsigned char[]> g_key = given_key->data();

      if (g_key >= c_key && g_key < n_key) {
        return i;
      }
      break;
    }
    }
  }

  return m_sort_keys.size() - 1;
}

void Summary::bytes(util::Buffer *buff) {
  // Filling buffer with the total number of pages.
  buff->put_int8(m_sort_keys.size());

  for (auto key : m_sort_keys) {
    // Filling buffer with data in bytes.
    key->bytes(buff);
  }
}