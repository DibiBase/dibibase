#include "catalog/types.hh"

#include <memory>

using namespace dibibase::catalog;

template <typename T> std::unique_ptr<DataType> DataType::from(Id id, T data) {
  switch (id) {
  case ASCII:
    return std::make_unique<ASCIIDataType>(data);
  case BIGINT:
    return std::make_unique<BigIntDataType>(data);
  case BOOLEAN:
    return std::make_unique<BooleanDataType>(data);
  case DOUBLE:
    return std::make_unique<DoubleDataType>(data);
  case FLOAT:
    return std::make_unique<FloatDataType>(data);
  case INT:
    return std::make_unique<IntDataType>(data);
  case SMALLINT:
    return std::make_unique<SmallIntDataType>(data);
  case TINYINT:
    return std::make_unique<TinyIntDataType>(data);
  default:
    throw undefined_data_type_error("Unknown data type");
  }
}

inline std::size_t ASCIIDataType::size() const { return sizeof(m_data); }
inline const char *ASCIIDataType::bytes() const { return m_data.c_str(); }

inline std::size_t BigIntDataType::size() const { return sizeof(m_data); }
inline const char *BigIntDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t BooleanDataType::size() const { return sizeof(m_data); }
inline const char *BooleanDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t DoubleDataType::size() const { return sizeof(m_data); }
inline const char *DoubleDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t FloatDataType::size() const { return sizeof(m_data); }
inline const char *FloatDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t IntDataType::size() const { return sizeof(m_data); }
inline const char *IntDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t SmallIntDataType::size() const { return sizeof(m_data); }
inline const char *SmallIntDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}

inline std::size_t TinyIntDataType::size() const { return sizeof(m_data); }
inline const char *TinyIntDataType::bytes() const {
  return reinterpret_cast<const char *>(&m_data);
}
