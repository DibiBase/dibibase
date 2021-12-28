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
    };

    Type(Id, size_t);

    static Type from(util::Buffer);

    static size_t get_size_from_id(Id);

    Id id() const;
    size_t size() const;

    std::unique_ptr<char[]> bytes() const;

  private:
    Id m_id;
    size_t m_size;
  };

public:
  explicit Data(Type type) : m_type(type) {}
  virtual ~Data();

  template <typename T> static std::unique_ptr<Data> from(T, Type);
  template <typename T> static std::unique_ptr<Data> from(T);

  Type type() const;

  virtual std::unique_ptr<char[]> bytes() const = 0;

private:
  Type m_type;
};

class DIBIBASE_PUBLIC ASCIIData : public Data {

public:
  explicit ASCIIData(std::string data)
      : Data(Type(Type::ASCII, data.size())), m_data(data) {}

  std::string data() const { return m_data; }
  void set_data(std::string data) { m_data = data; }

  std::unique_ptr<char[]> bytes() const override;

private:
  std::string m_data;
};

class DIBIBASE_PUBLIC BigIntData : public Data {

public:
  explicit BigIntData(uint64_t data)
      : Data(Type(Type::BIGINT, sizeof(data))), m_data(data) {}

  uint64_t data() const { return m_data; }
  void set_data(uint64_t data) { m_data = data; }

  std::unique_ptr<char[]> bytes() const override;

private:
  uint64_t m_data;
};

} // namespace dibibase::catalog
