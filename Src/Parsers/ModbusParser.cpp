// ModbusParser.cpp
#include "ModbusParser.h"


uint8_t ModbusParser::response_buffer[] = {0}; 

ModbusParser::ModbusParser(){
  //stateMachine = NULL;
}

parse_error_code ModbusParser::parseFrame(const volatile uint8_t* buffer, 
                                          uint16_t length){
  response_len = 0;

  if (length < MIN_MODBUS_IN) return PARSE_INVALID_LENGTH;
  if (buffer[0] != DataHandler::instance().getModbusRegister(0)){
    return PARSE_INVALID_ADDRESS;
  }
  
  uint8_t slave = buffer[0];
  uint8_t func = buffer[1];
  uint16_t crc_recv = (buffer[length - 1] << 8); 
  crc_recv |= buffer[length - 2];
  if (crc16(buffer, length - 2) != crc_recv) return PARSE_CRC_ERROR;

  if (func == 0x03){
    uint16_t start = (buffer[2] << 8); 
    start |= buffer[3];
    uint16_t count = (buffer[4] << 8); 
    count |= buffer[5];
      
    if (start + count > MAX_MODBUS_REG) return PARSE_INVALID_LENGTH;

    response_buffer[0] = slave;
    response_buffer[1] = 0x03;
    response_buffer[2] = count * 2;

    for (uint16_t i = 0; i < count; ++i) {
      uint16_t val = DataHandler::instance().getModbusRegister(start + i);
      response_buffer[3 + i * 2] = val >> 8;
      response_buffer[3 + i * 2 + 1] = val & 0xFF;
    }

    response_len = 3 + count * 2;
  }
  else if (func == 0x06){
    uint16_t addr = (buffer[2] << 8);
    addr |= buffer[3];
    uint16_t value = (buffer[4] << 8);
    value |= buffer[5];

    if (addr >= MAX_MODBUS_REG) return PARSE_INVALID_LENGTH;
    DataHandler::instance().setModbusRegister(addr, value);

    // Echo back the request
    response_len = 6;
    for (uint8_t i = 0; i < response_len; ++i){
      response_buffer[i] = buffer[i];
    }
  }
  else return PARSE_INVALID_FUNCTION;

  // Add CRC
  uint16_t crc = crc16(response_buffer, response_len);
  response_buffer[response_len++] = crc & 0xFF;
  response_buffer[response_len++] = crc >> 8;
  

  return PARSE_OK;
}

void ModbusParser::answer(uint8_t*& buffer, uint16_t& length) {
  buffer = response_buffer;
  length = response_len;
}


