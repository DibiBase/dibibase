#pragma once

#include <memory>
#include <unistd.h>
#include <utility>

namespace dibibase::util {

using std::forward;
using std::make_shared;
using std::shared_ptr;

template <class T, class... Deps> shared_ptr<T> get_singleton(Deps &&...deps) {
  static shared_ptr<T> instance{make_shared<T>(forward<Deps>(deps)...)};
  return instance;
}

} // namespace dibibase::util
