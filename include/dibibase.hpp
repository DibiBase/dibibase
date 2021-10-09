#pragma once

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DIBIBASE
#define DIBIBASE_PUBLIC __declspec(dllexport)
#else
#define DIBIBASE_PUBLIC __declspec(dllimport)
#endif
#else
#ifdef BUILDING_DIBIBASE
#define DIBIBASE_PUBLIC __attribute__((visibility("default")))
#else
#define DIBIBASE_PUBLIC
#endif
#endif

namespace dibibase {

class DIBIBASE_PUBLIC Dibibase {

public:
  Dibibase();
  int get_number() const;

private:
  int number;
};

} // namespace dibibase
