#pragma once

#include <cstdint>

#include "common.hpp"

namespace dibibase {
namespace api {
namespace cql3 {

enum FrameOpcode {
  ERROR = 0x00,
  STARTUP = 0x01,
  READY = 0x02,
  AUTHENTICATE = 0x03,
  OPTIONS = 0x05,
  SUPPORTED = 0x06,
  QUERY = 0x07,
  RESULT = 0x08,
  PREPARE = 0x09,
  EXECUTE = 0x0A,
  REGISTER = 0x0B,
  EVENT = 0x0C,
  BATCH = 0x0D,
  AUTH_CHALLENGE = 0x0E,
  AUTH_RESPONSE = 0x0F,
  AUTH_SUCCESS = 0x10
};

class DIBIBASE_PUBLIC DIBIBASE_PACKED FrameHeader {

public:
  explicit FrameHeader(char *);
  FrameHeader(const FrameHeader &) = default;
  FrameHeader &operator=(const FrameHeader &) = default;
  FrameHeader(FrameHeader &&) = default;
  FrameHeader &operator=(FrameHeader &&) = default;

  uint8_t getOpcode() const { return opcode; };

private:
  uint8_t version;
  uint8_t flags;
  uint16_t stream;
  uint8_t opcode;
  uint32_t length;
};

class DIBIBASE_PUBLIC Frame {

public:
  explicit Frame(FrameHeader header) : header{header} {};
  Frame(const Frame &) = default;
  Frame &operator=(const Frame &) = default;
  Frame(Frame &&) = default;
  Frame &operator=(Frame &&) = default;

  static Frame from(char *buf);

  FrameOpcode getOpCode() const noexcept;

  virtual ~Frame() = default;

private:
  FrameHeader header;
  FrameOpcode opcode;
};

class DIBIBASE_PUBLIC OptionsFrame : public Frame {

public:
  explicit OptionsFrame(FrameHeader header) : Frame{header} {};
  OptionsFrame(const OptionsFrame &) = default;
  OptionsFrame &operator=(const OptionsFrame &) = default;
  OptionsFrame(OptionsFrame &&) = default;
  OptionsFrame &operator=(OptionsFrame &&) = default;
};

} // namespace cql3
} // namespace api
} // namespace dibibase
