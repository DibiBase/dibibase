#pragma once

#include <vector>

#include "common.hh"
#include "mem/frame.hh"

namespace dibibase::mem {

class DIBIBASE_PUBLIC PageReplacementStrategy {

public:
  virtual void track(Frame) = 0;
  virtual void hit(Frame) = 0;
  virtual Frame evict(std::vector<Frame> &) = 0;
  virtual void remove(Frame) = 0;

private:
  virtual ~PageReplacementStrategy() = 0;
};

} // namespace dibibase::mem
