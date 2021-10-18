#include "api/cql3/frame.hh"

using namespace dibibase::api::cql3;

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
