#include "api/cql3/frame.hpp"

namespace dibibase {
namespace api {
namespace cql3 {

Frame Frame::from(char *buf) {
  FrameHeader header = FrameHeader(buf);
  FrameOpcode opcode = static_cast<FrameOpcode>(header.getOpcode() & 0xFE);

  switch (opcode) {
  case OPTIONS:
    return OptionsFrame(header);
  default:
    return Frame(header);
  }
}

} // namespace cql3
} // namespace api
} // namespace dibibase