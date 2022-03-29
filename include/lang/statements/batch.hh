#pragma once

#include <string>
#include <vector>

#include "common.hh"
#include "lang/statements/statement.hh"
#include "lang/statements/timestamp_spec.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC Batch {

public:
  enum DIBIBASE_PUBLIC Type { LOGGED, UNLOGGED, UNVALID };

public:
  Batch() {}

  static Batch from_type(Type type) {
    Batch batch;

    batch.m_valid = true;
    batch.m_type = type;

    return batch;
  }

  static Batch from_timestamp_spec(TimestampSpec timestamp_spec) {
    Batch batch;

    batch.m_valid = true;
    batch.m_timestamp_spec = timestamp_spec;

    return batch;
  }

  static Batch from_type_and_timestamp_spec(Type type,
                                            TimestampSpec timestamp_spec) {
    Batch batch;

    batch.m_valid = true;
    batch.m_type = type;
    batch.m_timestamp_spec = timestamp_spec;
    
    return batch;
  }

public:
  bool m_valid;
  Type m_type;
  TimestampSpec m_timestamp_spec;
};

} // namespace dibibase::lang
