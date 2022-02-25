#include "catalog/data.hh"

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
  case Type::BLOB: {
    std::unique_ptr<unsigned char[]> data = buf->get_blob(type.size());
    return std::make_unique<BlobData>(std::move(data), type.size());
  }
  }

  return nullptr;
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

void ASCIIData::bytes(util::Buffer *buf) { buf->put_string(m_data); }

BigIntData::BigIntData(int64_t data)
    : Data(Type(Type::BIGINT, sizeof(data))), m_data(data) {}

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
    : Data(Type(Type::BOOLEAN, sizeof(data))), m_data(data) {}

bool BooleanData::compare(Data *other __attribute__((unused))) {
  throw uncomparable_type_error("");
}

void BooleanData::bytes(util::Buffer *buf) { buf->put_boolean(m_data); }

DoubleData::DoubleData(double data)
    : Data(Type(Type::DOUBLE, sizeof(data))), m_data(data) {}

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
    : Data(Type(Type::FLOAT, sizeof(data))), m_data(data) {}

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
    : Data(Type(Type::INT, sizeof(data))), m_data(data) {}

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
    : Data(Type(Type::SMALLINT, sizeof(data))), m_data(data) {}

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
    : Data(Type(Type::TINYINT, sizeof(data))), m_data(data) {}

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

void BlobData::bytes(util::Buffer *buf) { buf->put_blob(m_data.get(), m_size); }
