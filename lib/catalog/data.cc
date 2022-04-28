#include "catalog/data.hh"

#include <cstdint>
#include <iostream>
#include <memory>

using namespace dibibase::catalog;

std::unique_ptr<Data> Data::from(util::Buffer *buf, Type type) {
  Type::Id id = type.id();

  switch (id) {
  case Type::ASCII: {
    uint8_t number_of_bytes = buf->get_uint8();
    std::string data = buf->get_string(number_of_bytes);
    return std::make_unique<ASCIIData>(data);
  }
  case Type::BIGINT: {
    int64_t data = buf->get_int64();
    return std::make_unique<BigIntData>(data);
  }
  case Type::BOOLEAN: {
    bool data = buf->get_boolean();
    return std::make_unique<BooleanData>(data);
  }
  case Type::DOUBLE: {
    double data = buf->get_double();
    return std::make_unique<DoubleData>(data);
  }
  case Type::FLOAT: {
    float data = buf->get_float();
    return std::make_unique<FloatData>(data);
  }
  case Type::INT: {
    int32_t data = buf->get_int32();
    return std::make_unique<IntData>(data);
  }
  case Type::SMALLINT: {
    int16_t data = buf->get_int16();
    return std::make_unique<SmallIntData>(data);
  }
  case Type::TINYINT: {
    int8_t data = buf->get_int8();
    return std::make_unique<TinyIntData>(data);
  }
  case Type::BLOB: {
    std::unique_ptr<unsigned char[]> data = buf->get_blob(type.length());
    return std::make_unique<BlobData>(std::move(data), type.length());
  }
  }

  return nullptr;
}

std::unique_ptr<Data> Data::from(catalog::Data *data) {
  switch (data->type().id()) {
  case Type::ASCII: {
    if (ASCIIData *rhs = dynamic_cast<ASCIIData *>(data); rhs != nullptr)
      return std::make_unique<ASCIIData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::BIGINT: {
    if (BigIntData *rhs = dynamic_cast<BigIntData *>(data); rhs != nullptr)
      return std::make_unique<BigIntData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::BOOLEAN: {
    if (BooleanData *rhs = dynamic_cast<BooleanData *>(data); rhs != nullptr)
      return std::make_unique<BooleanData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::DOUBLE: {
    if (DoubleData *rhs = dynamic_cast<DoubleData *>(data); rhs != nullptr)
      return std::make_unique<DoubleData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::FLOAT: {
    if (FloatData *rhs = dynamic_cast<FloatData *>(data); rhs != nullptr)
      return std::make_unique<FloatData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::INT: {
    if (IntData *rhs = dynamic_cast<IntData *>(data); rhs != nullptr)
      return std::make_unique<IntData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::SMALLINT: {
    if (SmallIntData *rhs = dynamic_cast<SmallIntData *>(data); rhs != nullptr)
      return std::make_unique<SmallIntData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::TINYINT: {
    if (TinyIntData *rhs = dynamic_cast<TinyIntData *>(data); rhs != nullptr)
      return std::make_unique<TinyIntData>(rhs->data());

    throw undefined_data_type_error("");
  }
  case Type::BLOB: {
    if (BlobData *rhs = dynamic_cast<BlobData *>(data); rhs != nullptr)
      return std::make_unique<BlobData>(rhs->data(), rhs->length());

    throw undefined_data_type_error("");
  }
  }

  throw undefined_data_type_error("");
}

ASCIIData::ASCIIData(std::string data)
    : Data(Type(Type::ASCII, data.size())), m_data(data) {}

bool ASCIIData::compare(Data *other) {
  if (ASCIIData *rhs = dynamic_cast<ASCIIData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void ASCIIData::bytes(util::Buffer *buf) { 
  buf->put_uint8(m_data.size());
  buf->put_string(m_data);
}

BigIntData::BigIntData(int64_t data)
    : Data(Type(Type::BIGINT, sizeof(int64_t))), m_data(data) {}

bool BigIntData::compare(Data *other) {
  if (BigIntData *rhs = dynamic_cast<BigIntData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void BigIntData::bytes(util::Buffer *buf) { buf->put_int64(m_data); }

BooleanData::BooleanData(bool data)
    : Data(Type(Type::BOOLEAN, sizeof(bool))), m_data(data) {}

bool BooleanData::compare(Data *other __attribute__((unused))) {
  throw uncomparable_type_error("");
}

void BooleanData::bytes(util::Buffer *buf) { buf->put_boolean(m_data); }

DoubleData::DoubleData(double data)
    : Data(Type(Type::DOUBLE, sizeof(double))), m_data(data) {}

bool DoubleData::compare(Data *other) {
  if (DoubleData *rhs = dynamic_cast<DoubleData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void DoubleData::bytes(util::Buffer *buf) { buf->put_double(m_data); }

FloatData::FloatData(float data)
    : Data(Type(Type::FLOAT, sizeof(float))), m_data(data) {}

bool FloatData::compare(Data *other) {
  if (FloatData *rhs = dynamic_cast<FloatData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void FloatData::bytes(util::Buffer *buf) { buf->put_float(m_data); }

IntData::IntData(int32_t data)
    : Data(Type(Type::INT, sizeof(int32_t))), m_data(data) {}

bool IntData::compare(Data *other) {
  if (IntData *rhs = dynamic_cast<IntData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void IntData::bytes(util::Buffer *buf) { buf->put_int32(m_data); }

SmallIntData::SmallIntData(int16_t data)
    : Data(Type(Type::SMALLINT, sizeof(int16_t))), m_data(data) {}

bool SmallIntData::compare(Data *other) {
  if (SmallIntData *rhs = dynamic_cast<SmallIntData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void SmallIntData::bytes(util::Buffer *buf) { buf->put_int16(m_data); }

TinyIntData::TinyIntData(int8_t data)
    : Data(Type(Type::TINYINT, sizeof(int8_t))), m_data(data) {}

bool TinyIntData::compare(Data *other) {
  if (TinyIntData *rhs = dynamic_cast<TinyIntData *>(other); rhs != nullptr) {
    if (m_data <= rhs->data())
      return false;

    return true;
  }

  throw incompatible_data_types_error("");
}

void TinyIntData::bytes(util::Buffer *buf) { buf->put_int8(m_data); }

BlobData::BlobData(std::unique_ptr<unsigned char[]> data, size_t size)
    : Data(Type(Type::BLOB, size)), m_size(size), m_data(std::move(data)) {}

bool BlobData::compare(Data *other __attribute__((unused))) {
  throw uncomparable_type_error("");
}

std::unique_ptr<unsigned char[]> BlobData::data() const {
  std::unique_ptr<unsigned char[]> data =
      std::make_unique<unsigned char[]>(m_size);

  // Copying data content.
  for (int i = 0; i < (int)m_size; ++i) {
    data[i] = m_data[i];
  }
  return data;
}

void BlobData::set_data(unsigned char *data, size_t size) {
  if (size > m_size) {
    throw "error";
  }
  memcpy(m_data.get(), data, size);
}

size_t BlobData::length() const { return m_size; }

void BlobData::bytes(util::Buffer *buf) { buf->put_blob(m_data.get(), m_size); }
