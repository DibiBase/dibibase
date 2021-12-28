#include "catalog/data.hh"
#include <memory>
#include <iostream>

using namespace dibibase::catalog;

std::unique_ptr<Data> Data::from(util::Buffer *buf, Type type) {
  Type::Id id = type.id();

  switch (id) {
  case Type::ASCII: {
    std::string data = buf->get_string(type.size());
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
  }

  return nullptr;
}

ASCIIData::ASCIIData(std::string data)
    : Data(Type(Type::ASCII, data.size())), m_data(data) {}

void ASCIIData::bytes(util::Buffer* buf) { buf->put_string(m_data); }

BigIntData::BigIntData(int64_t data)
    : Data(Type(Type::BIGINT, sizeof(data))), m_data(data) {}

void BigIntData::bytes(util::Buffer* buf) { buf->put_int64(m_data); }

BooleanData::BooleanData(bool data)
    : Data(Type(Type::BOOLEAN, sizeof(data))), m_data(data) {}

void BooleanData::bytes(util::Buffer* buf) {
  buf->put_boolean(m_data);
}

DoubleData::DoubleData(double data)
    : Data(Type(Type::DOUBLE, sizeof(data))), m_data(data) {}

void DoubleData::bytes(util::Buffer* buf) {
  buf->put_double(m_data);
}

FloatData::FloatData(float data)
    : Data(Type(Type::FLOAT, sizeof(data))), m_data(data) {}

void FloatData::bytes(util::Buffer* buf) { buf->put_float(m_data); }

IntData::IntData(int32_t data)
    : Data(Type(Type::INT, sizeof(data))), m_data(data) {}

void IntData::bytes(util::Buffer* buf) { buf->put_int32(m_data); }

SmallIntData::SmallIntData(int16_t data)
    : Data(Type(Type::SMALLINT, sizeof(data))), m_data(data) {}

void SmallIntData::bytes(util::Buffer* buf) {
  buf->put_int16(m_data);
}

TinyIntData::TinyIntData(int8_t data)
    : Data(Type(Type::TINYINT, sizeof(data))), m_data(data) {}

void TinyIntData::bytes(util::Buffer* buf) { buf->put_int8(m_data); }