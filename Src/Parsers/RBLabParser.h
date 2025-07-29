// RBLabParser.h
#pragma once

#include "main.h"
#include "IParser.h"
//#include "DataHandler.h"
#include "Channels.h"
#include "Mediator.h"
#include "Singleton.h"
#include "RtcTask.h"
#include "Config.h"

namespace RBLabCommand {
  enum Type {
    NONE = 0,
    TO_RBLAB_MODE = 0x05,
    FROM_TRANS_MODE = 0x04,
    FROM_RBLAB_MODE = 0x19,
    TO_TRANS_MODE = 0x32,
    SET_TIME = 0x33,
    ASK_PTH = 0x3b,
    WRITE_PARAM = 0x3c,
    ASK_PARAM = 0x3d,
    READ_VOLT = 0x47,
    READ_CONC = 0x49,
    WRITE_CONFIG = 0xd0,
    READ_CONFIG = 0xd1,
    WRITE_SENSOR_CONFIG = 0xd2,
    READ_SENSOR_CONFIG = 0xd3,
  };
}


class RBLabParser : public IParser, public Singleton<RBLabParser> {
  friend class Singleton<RBLabParser>;
  
public:

  virtual parse_error_code parseFrame
    (const volatile uint8_t* buffer, uint16_t length);
  
  virtual void answer(uint8_t*& buffer, uint16_t& length);
  
  RBLabCommand::Type readCommandToMCU();
  
private:
  static const uint8_t MAX_RBLAB_OUT = UARTS_BUFFER_SIZE;
  
  STATIC_ASSERT(MAX_RBLAB_OUT > MAX_SENSORS*(ASCII_FLOAT_SIZE + 4) + 6,
              size_of_rblab_buffer_must_be_large_enough);
  
  RBLabParser();
  
  void setTransChannel(uint8_t ch_index); 
  
  void crcToAscii(uint8_t* source, uint16_t length, uint8_t* crc_buffer);
  
  void floatToAscii(float val, uint8_t* conc_buffer); 
  
  void intToAscii(int16_t val, uint8_t* conc_buffer);
  
  int16_t asciiToInt(const volatile uint8_t buffer[4], bool& valid);
  
  void addCrcAndNull(uint8_t* buffer, uint16_t& len);
  
  parse_error_code callSpecificParser
    (uint8_t cmd, const volatile uint8_t* buffer,uint16_t length);
  
  parse_error_code fixedSizeCommand(const volatile uint8_t* buffer, 
                                    uint16_t length, 
                                    const uint8_t* validPattern);
  
  parse_error_code toTransModeParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code fromTransModeParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code fromRbLabModeParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code setTime
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code writeParamParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code writeConfigParse
    (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code writeSensorConfigParse
     (const volatile uint8_t* buffer, uint16_t length);
  
  parse_error_code readSensorConfigParse
     (const volatile uint8_t* buffer, uint16_t length);
  
  
  void enterTransMode(uint8_t trans_channel);
  
  void exitTransparentMode();
  
  void exitRbLabMode();
  
  void enterRbLabMode();
  
  // Global buffer to hold the RBLab response
  static uint8_t response_buffer[MAX_RBLAB_OUT]; 
  uint16_t response_len;
  uint8_t trans_channel;
  uint8_t read_channel;
  
  RBLabCommand::Type commandFromPC;
  RBLabCommand::Type commandToMCU;
  
public:
  struct CommandEntry {
    // Command identifier
    RBLabCommand::Type cmd;         
    // Expected message (NULL if handled by specific parser)
    const uint8_t* pattern; 
    // Call enterRbLabMode() if true
    bool enterMode;        
  };
  
};