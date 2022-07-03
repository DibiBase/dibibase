#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "common.hh"
#include "errors.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

DEFINE_ERROR(undefined_data_type_error);
DEFINE_ERROR(incompatible_data_types_error);
DEFINE_ERROR(uncomparable_type_error);

class DIBIBASE_PUBLIC Data {

public:
  class DIBIBASE_PUBLIC Type {

  public:
    /**
     * @brief An enumeration of the supported data types in the CatalogManager.
     */
    enum DIBIBASE_PUBLIC Id {
      ASCII,    /**< ASCII character string */
      BIGINT,   /**< 64-bit signed long */
      BOOLEAN,  /**< Either true or false */
      DOUBLE,   /**< 64-bit IEEE-754 floating point */
      FLOAT,    /**< 32-bit IEEE-754 floating point */
      INT,      /**< 32-bit signed int */
      SMALLINT, /**< 16-bit signed int */
      TINYINT,  /**< 8-bit signed int */
      BLOB,     /**<Binary large object*/
    };

    Type(Id id, uint32_t length) : m_id(id), m_length(length) {}

    static std::unique_ptr<Type> from(util::Buffer *buf) {
      uint8_t id = buf->get_int8();
      uint32_t size = buf->get_int32();
      return std::make_unique<Type>(static_cast<Id>(id), size);
    }

    constexpr Id id() const { return m_id; }
    constexpr size_t length() const { return m_length; }

    size_t size() const { return sizeof(uint8_t) + sizeof(uint32_t); }
    void bytes(util::Buffer *buf) {
      buf->put_int8(m_id);
      buf->put_uint32(m_length);
    }

  private:
    Id m_id;
    uint32_t m_length;
  };

public:
  explicit Data(Type type) : m_type(type) {}
  virtual ~Data() {}

  static std::unique_ptr<Data> from(util::Buffer *, Type);
  static std::unique_ptr<Data> from(catalog::Data *);

  virtual bool compare(Data *other) = 0;
  virtual bool is_equal(Data *other) = 0;

  virtual std::string print() = 0;

  Type type() const { return m_type; }

  virtual void bytes(util::Buffer *) = 0;

private:
  Type m_type;
};

constexpr bool operator==(const Data::Type &lhs, const Data::Type &rhs) {
  return lhs.id() == rhs.id() && lhs.length() == rhs.length();
}

constexpr bool operator!=(const Data::Type &lhs, const Data::Type &rhs) {
  return !(lhs == rhs);
}

class DIBIBASE_PUBLIC ASCIIData : public Data {

public:
  explicit ASCIIData(std::string data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return data(); }

  std::string data() const { return m_data; }
  void set_data(std::string data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  std::string m_data;
};

class DIBIBASE_PUBLIC BigIntData : public Data {

public:
  explicit BigIntData(int64_t data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  int64_t data() const { return m_data; }
  void set_data(int64_t data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  int64_t m_data;
};

class DIBIBASE_PUBLIC BooleanData : public Data {

public:
  explicit BooleanData(bool data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  bool data() const { return m_data; }
  void set_data(bool data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  bool m_data;
};

class DIBIBASE_PUBLIC DoubleData : public Data {

public:
  explicit DoubleData(double data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  double data() const { return m_data; }
  void set_data(double data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  double m_data;
};

class DIBIBASE_PUBLIC FloatData : public Data {

public:
  explicit FloatData(float data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  float data() const { return m_data; }
  void set_data(float data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  float m_data;
};

class DIBIBASE_PUBLIC IntData : public Data {

public:
  explicit IntData(int32_t data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  int32_t data() const { return m_data; }
  void set_data(int32_t data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  int32_t m_data;
};

class DIBIBASE_PUBLIC SmallIntData : public Data {

public:
  explicit SmallIntData(int16_t data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  int16_t data() const { return m_data; }
  void set_data(int16_t data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  int16_t m_data;
};

class DIBIBASE_PUBLIC TinyIntData : public Data {

public:
  explicit TinyIntData(int8_t data);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override { return std::to_string(data()); }

  int8_t data() const { return m_data; }
  void set_data(int8_t data) { m_data = data; }

  void bytes(util::Buffer *) override;

private:
  int8_t m_data;
};

class DIBIBASE_PUBLIC BlobData : public Data {

public:
  explicit BlobData(std::unique_ptr<unsigned char[]>, size_t);

  bool compare(Data *other) override;
  bool is_equal(Data *other) override;

  std::string print() override {}

  std::unique_ptr<unsigned char[]> data() const;
  void set_data(unsigned char *, size_t);

  size_t length() const;

  void bytes(util::Buffer *) override;

private:
  size_t m_size;
  std::unique_ptr<unsigned char[]> m_data;
};

struct DataCmp {
  bool operator()(const std::unique_ptr<Data> &lhs,
                  const std::unique_ptr<Data> &rhs) const {
    return lhs->compare(rhs.get());
  }

  bool operator()(const std::shared_ptr<Data> &lhs,
                  const std::shared_ptr<Data> &rhs) const {
    return lhs->compare(rhs.get());
  }
};

} // namespace dibibase::catalog
