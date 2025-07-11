// IParser.h
#pragma once

#include "main.h"
#include "StateMachine.h"
// Error codes used across task-related interfaces
typedef enum {
  PARSE_OK = 0,
  PARSE_CRC_ERROR,
  PARSE_ZERO_SET_ERROR,
  PARSE_INVALID_FUNCTION,
  PARSE_INVALID_ADDRESS,
  PARSE_INVALID_FORMAT,
  PARSE_UNKNOWN_MESSAGE,
  PARSE_INVALID_LENGTH,
  PARSE_NO_TRANS_MESS,
  PARSE_DELAY
} parse_error_code;

class IParser {
public:
  virtual ~IParser() {}
  
  // Called when a full frame is received.
  // Should fill internal response buffer and return pointer and length.
  virtual parse_error_code parseFrame
    (const volatile uint8_t* request, uint16_t length) = 0;
  virtual void answer(uint8_t*& buffer, uint16_t& length) = 0;
  
  virtual void setStateMachine(StateMachine* stateMachine){
    this->stateMachine = stateMachine;
  }
  
protected:
  static const uint8_t ASCII_FLOAT_SIZE = 8;
    // Function to check CRC
  virtual uint16_t crc16(const volatile uint8_t *data, uint16_t length){
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; ++i) {
      crc ^= data[i];
      for (uint8_t j = 0; j < 8; ++j) {
        if (crc & 1) crc = (crc >> 1) ^ 0xA001;  
        else crc >>= 1;
      }
    }
    return crc;
  }
  
      // convert 0xEBD6 -> ['d','6','e','b']
  virtual uint16_t asciiHexToCrc(const volatile uint8_t *inputBuffer) {

    uint16_t value = 0;

    for (int i = 0; i < 4; ++i) {
      char c = inputBuffer[i];
      uint8_t nibble;

      if (c >= '0' && c <= '9')
        nibble = c - '0';
      else if (c >= 'a' && c <= 'f')
        nibble = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F') 
        nibble = c - 'A' + 10;
      else
        return 0;

      value = (value << 4) | nibble;
    }
    // big-endian to little-endian
    return ((value & 0x00FF) << 8) | (value >> 8);
  }

  
  StateMachine* stateMachine;
};