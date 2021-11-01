#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "common.hh"
#include "errors.hh"

namespace dibibase::catalog {

DEFINE_ERROR(undefined_data_type_error);

class DIBIBASE_PUBLIC DataType {

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

  DataType() = delete;

  template <typename T> static std::unique_ptr<DataType> from(Id, T);
  static std::unique_ptr<DataType> from(char *);

  virtual size_t size() const = 0;
  virtual const char *bytes() const = 0;

  virtual ~DataType();
};

class DIBIBASE_PUBLIC ASCIIDataType : public DataType {

public:
  explicit ASCIIDataType(std::string);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline std::string data() { return m_data; }
  inline void set_data(std::string data) { m_data = data; }

private:
  std::string m_data;
};

class DIBIBASE_PUBLIC BigIntDataType : public DataType {

public:
  explicit BigIntDataType(int64_t);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline int64_t data() { return m_data; }
  inline void set_data(int64_t data) { m_data = data; }

private:
  int64_t m_data;
};

class DIBIBASE_PUBLIC BooleanDataType : public DataType {

public:
  explicit BooleanDataType(bool);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline bool data() { return m_data; }
  inline void set_data(bool data) { m_data = data; }

private:
  bool m_data;
};

class DIBIBASE_PUBLIC DoubleDataType : public DataType {

public:
  explicit DoubleDataType(double);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline double data() { return m_data; }
  inline void set_data(double data) { m_data = data; }

private:
  double m_data;
};

class DIBIBASE_PUBLIC FloatDataType : public DataType {

public:
  explicit FloatDataType(float);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline float data() { return m_data; }
  inline void set_data(float data) { m_data = data; }

private:
  float m_data;
};

class DIBIBASE_PUBLIC IntDataType : public DataType {

public:
  explicit IntDataType(int32_t);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline int32_t data() { return m_data; }
  inline void set_data(int32_t data) { m_data = data; }

private:
  int32_t m_data;
};

class DIBIBASE_PUBLIC SmallIntDataType : public DataType {

public:
  explicit SmallIntDataType(int16_t);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline int16_t data() { return m_data; }
  inline void set_data(int16_t data) { m_data = data; }

private:
  int16_t m_data;
};

class DIBIBASE_PUBLIC TinyIntDataType : public DataType {

public:
  explicit TinyIntDataType(int16_t);

  inline size_t size() const override;
  inline const char *bytes() const override;

  inline int8_t data() { return m_data; }
  inline void set_data(int8_t data) { m_data = data; }

private:
  int8_t m_data;
};

} // namespace dibibase::catalog
