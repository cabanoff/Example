// ModbusParser.h
#pragma once

#include "main.h"
#include "IParser.h"
#include "DataHandler.h"
#include "Singleton.h"

class ModbusParser : public IParser, public Singleton<ModbusParser> {
  friend class Singleton<ModbusParser>;
public:

  virtual parse_error_code parseFrame(const volatile uint8_t* buffer, 
                                    uint16_t length);
  virtual void answer(uint8_t*& buffer, uint16_t& length);

private:
  
  static const uint8_t MAX_MODBUS_REG = MODBUS_REGS;
  // addr(1), func(1), bytes_count(1), data(37*2)+ CRC(2)  
  static const uint8_t MAX_MODBUS_OUT = MAX_MODBUS_REG*2 + 5;
  // Minimum length (addr(1), func(1), start(2), count(2), CRC(2))
  static const uint8_t MIN_MODBUS_IN = 8;
  
  ModbusParser();
   
  // Global buffer to hold the Modbus response
  static uint8_t response_buffer[MAX_MODBUS_OUT]; 
  uint16_t response_len;
};