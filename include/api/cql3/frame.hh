#pragma once

#include <cstdint>

#include "common.hh"

namespace dibibase::api::cql3 {

/**
 * @brief An enum containing possible Opcodes for Frames.
 */
enum FrameOpcode {
  ERROR = 0x00,          /**< enum value ERROR. */
  STARTUP = 0x01,        /**< enum value STARTUP. */
  READY = 0x02,          /**< enum value READY. */
  AUTHENTICATE = 0x03,   /**< enum value AUTHENTICATE. */
  OPTIONS = 0x05,        /**< enum value OPTIONS. */
  SUPPORTED = 0x06,      /**< enum value SUPPORTED. */
  QUERY = 0x07,          /**< enum value QUERY. */
  RESULT = 0x08,         /**< enum value RESULT. */
  PREPARE = 0x09,        /**< enum value PREPARE. */
  EXECUTE = 0x0A,        /**< enum value EXECUTE. */
  REGISTER = 0x0B,       /**< enum value REGISTER. */
  EVENT = 0x0C,          /**< enum value EVENT. */
  BATCH = 0x0D,          /**< enum value BATCH. */
  AUTH_CHALLENGE = 0x0E, /**< enum value AUTH_CHALLENGE. */
  AUTH_RESPONSE = 0x0F,  /**< enum value AUTH_RESPONSE. */
  AUTH_SUCCESS = 0x10    /**< enum value AUTH_SUCCESS. */
};

/**
 * @brief An class representing Frame Headers.
 */
class DIBIBASE_PUBLIC DIBIBASE_PACKED FrameHeader {

public:
  /**
   * @brief A constructor for initializing Frame Headers.
   * @param header the buffer from which to read the header.
   */
  explicit FrameHeader(char *);
  FrameHeader(const FrameHeader &) = default;
  FrameHeader &operator=(const FrameHeader &) = default;
  FrameHeader(FrameHeader &&) = default;
  FrameHeader &operator=(FrameHeader &&) = default;

  /**
   * @brief Opcode Getter.
   * @return The Opcode
   */
  uint8_t getOpcode() const { return opcode; };

private:
  uint8_t version;
  uint8_t flags;
  uint16_t stream;
  uint8_t opcode;
  uint32_t length;
};

/**
 * @brief A class representing CQL3 binary Frames.
 */
class DIBIBASE_PUBLIC Frame {

public:
  /**
   * @brief A base constructor to be called by child classes.
   * @param header the frame header.
   */
  explicit Frame(FrameHeader header) : header{header} {};
  Frame(const Frame &) = default;
  Frame &operator=(const Frame &) = default;
  Frame(Frame &&) = default;
  Frame &operator=(Frame &&) = default;

  /**
   * @brief A factory method for creating all frame types.
   * @param buf the buffer from which to create the frame.
   * @see Frame()
   * @return The test results
   */
  static Frame from(char *buf);

  virtual ~Frame() = default;

private:
  FrameHeader header;
};

/**
 * @brief A class representing the Options Opcode Frame.
 */
class DIBIBASE_PUBLIC OptionsFrame : public Frame {

public:
  /**
   * @brief A constructor to read the body and initialize superclass.
   * @param header the frame header.
   */
  explicit OptionsFrame(FrameHeader header) : Frame{header} {};
  OptionsFrame(const OptionsFrame &) = default;
  OptionsFrame &operator=(const OptionsFrame &) = default;
  OptionsFrame(OptionsFrame &&) = default;
  OptionsFrame &operator=(OptionsFrame &&) = default;
};

} // namespace dibibase::api::cql3
