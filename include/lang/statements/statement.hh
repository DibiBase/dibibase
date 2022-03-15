#pragma once

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC Statement {

public:
  /**
   * @brief An enumeration of all CQL3 statement types.
   */
  enum DIBIBASE_PUBLIC Type {
    ALTER_KEYSPACE,
    ALTER_MATERIALIZED_VIEW,
    ALTER_ROLE,
    ALTER_TABLE,
    ALTER_TYPE,
    ALTER_USER,
    APPLY_BATCH,
    CREATE_AGGREGATE,
    CREATE_FUNCTION,
    CREATE_INDEX,
    CREATE_KEYSPACE,
    CREATE_MATERIALIZED_VIEW,
    CREATE_ROLE,
    CREATE_TABLE,
    CREATE_TRIGGER,
    CREATE_TYPE,
    CREATE_USER,
    DELETE,
    DROP_AGGREGATE,
    DROP_FUNCTION,
    DROP_INDEX,
    DROP_KEYSPACE,
    DROP_MATERIALIZED_VIEW,
    DROP_ROLE,
    DROP_TABLE,
    DROP_TRIGGER,
    DROP_TYPE,
    DROP_USER,
    GRANT,
    INSERT,
    LIST_PERMISSIONS,
    LIST_ROLES,
    REVOKE,
    SELECT,
    TRUNCATE,
    UPDATE,
    USE
  };

public:
  explicit Statement(Type type) : m_type(type) {}
  virtual ~Statement() {}

  Type type() const { return m_type; }

private:
  Type m_type;
};

} // namespace dibibase::lang
