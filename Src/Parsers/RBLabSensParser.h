// RBLabSensParser.h
#pragma once

#include "main.h"
#include "IParser.h"
#include "DataHandler.h"
#include "Singleton.h"

namespace RBLabSensCommand {
  enum Type {
    NONE = 0,
    ASK_SUBST_NAME = 0x87,
    ASK_VOLT = 0xad,
    ASK_CONC = 0xb6,
    ZERO_SET = 0x8d,
    ASK_ZERO_END = 0xac
  };
}

namespace RBLabSensResponce {
  enum Type {
    NONE = 0,
    MEASUREMENT = 0x88,
    ZERO_SET_START = 0xfb,
    ZERO_SET_ERROR = 0xfa
  };
}

class RBLabSensParser : public IParser, public Singleton<RBLabSensParser> {
  friend class Singleton<RBLabSensParser>;
  
public:

  virtual parse_error_code parseFrame(const volatile uint8_t* buffer, 
                                      uint16_t length);
  
  virtual void answer(uint8_t*& buffer, uint16_t& length);
  
  void setChannel(uint8_t ch_index);

private:
  static const uint8_t MAX_RBLAB_SENS_OUT = 64;
  
  RBLabSensParser();

  parse_error_code fromSensWorkMode
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code measurementsParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  bool setValue
    (const volatile uint8_t* buffer, RBLabSensCommand::Type type );
  
  bool setZero(const volatile uint8_t* buffer); 
  
  uint8_t hexCharToNibble(uint8_t c);
  
  // Global buffer to hold the RBLab response
  static uint8_t response_buffer[MAX_RBLAB_SENS_OUT]; 
  uint16_t response_len;
  uint8_t channel;
  
  static const uint8_t  ask_subst_name[7];
  static const uint8_t  ask_conc_volt[8];
  static const uint8_t  zero_set[6];
  static const uint8_t  ask_zero_set_end[7];
};