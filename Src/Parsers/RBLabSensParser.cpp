// RBLabSensParser.cpp

#include "RBLabSensParser.h"


uint8_t RBLabSensParser::response_buffer[] = {0}; 

const uint8_t  RBLabSensParser::ask_subst_name[] = \
  {0x87, 0x87, 0x32, 0x32, 0x32, 0x32, 0x00};
const uint8_t  RBLabSensParser::ask_conc_volt[] = \
  {0x87, 0xb6, 0xad, 0x37, 0x36, 0x33, 0x34, 0x00};
const uint8_t  RBLabSensParser::zero_set[] = \
  {0x8d, 0x37, 0x66, 0x32, 0x35, 0x00};
const uint8_t  RBLabSensParser::ask_zero_set_end[] = \
  {0x87, 0xac, 0x36, 0x32, 0x33, 0x64, 0x00};


RBLabSensParser::RBLabSensParser():channel(0){
  stateMachine = NULL;
}


void RBLabSensParser::setChannel(uint8_t ch_index){
  channel = ch_index;
}

parse_error_code RBLabSensParser::parseFrame(const volatile uint8_t* buffer, 
                                         uint16_t length){
                                           
  if(length < 6) return PARSE_INVALID_LENGTH;
  if(buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;

  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
    
    switch(buffer[0]){
    case RBLabSensResponce::MEASUREMENT:
      return fromSensWorkMode(&buffer[1],length - 6);
    case RBLabSensResponce::ZERO_SET_START:
      return PARSE_OK;
    case RBLabSensResponce::ZERO_SET_ERROR:
      return PARSE_ZERO_SET_ERROR;
    default:
      return PARSE_UNKNOWN_MESSAGE;
    }
  }
  else return PARSE_CRC_ERROR;
                                           
}



//88 [87 d3 e3 eb e5 f0 ee e4 e0 20 ee ea f1 e8 e4 0a] 65 62 64 36 00
//88 [ac 30 31 30 30] 63 34 35 30 00
parse_error_code RBLabSensParser::fromSensWorkMode
(const volatile uint8_t* buffer, uint16_t length){
  if(length < 3) return PARSE_INVALID_LENGTH;
  RBLabSensCommand::Type title;
  title = static_cast<RBLabSensCommand::Type>(buffer[0]);
  switch(title){
    
  case RBLabSensCommand::ASK_SUBST_NAME:  //substance name from sensor
    if(buffer[length - 1]!= 0x0a) return PARSE_INVALID_FORMAT;
    if(DataHandler::instance().setName(channel, &buffer[1], length - 2)){
      return  PARSE_OK;
    }
    else return PARSE_INVALID_FORMAT;
    
  case RBLabSensCommand::ASK_CONC:
  case RBLabSensCommand::ASK_VOLT: 
    
    if((length % (ASCII_FLOAT_SIZE + 1)) != 0) return PARSE_INVALID_LENGTH;
    return measurementsParse(buffer, length);
      
  case RBLabSensCommand::ASK_ZERO_END: // [ac 30 31 30 30]||[ac 30 32 38 30]
    
    if(length != 5) return PARSE_INVALID_LENGTH;
    if(setZero(&buffer[1])) return PARSE_OK;
    
  default:
    return PARSE_UNKNOWN_MESSAGE;
  }
}

//here we know that buffer contains 9 or 18 bytes
// 0xb6 _ _ _ _ _ _ _ _ 0xad _ _ _ _ _ _ _ _

parse_error_code RBLabSensParser::measurementsParse
(const volatile uint8_t* buffer, uint16_t length){
  
  RBLabSensCommand::Type type = static_cast<RBLabSensCommand::Type>(buffer[0]);
  if(type == RBLabSensCommand::ASK_CONC){
    if(!setValue(&buffer[1],RBLabSensCommand::ASK_CONC))
      return PARSE_INVALID_FORMAT;
  }
  else{
    if(!setValue(&buffer[1],RBLabSensCommand::ASK_VOLT))
      return PARSE_INVALID_FORMAT;
  }
  length -= ASCII_FLOAT_SIZE + 1;
  if(length){
    return fromSensWorkMode(&buffer[ASCII_FLOAT_SIZE + 1],length);
  }
  else return PARSE_OK;
}


bool RBLabSensParser::setValue
(const volatile uint8_t* buffer, RBLabSensCommand::Type type ){
  
  if (!buffer) return false;
  
  union ascii_float{
    float f;
    uint8_t bytes[4];
  } value;

  for (int i = 0; i < 4; ++i){
    uint8_t high = hexCharToNibble(buffer[i * 2]);
    uint8_t low  = hexCharToNibble(buffer[i * 2 + 1]);
    if (high == 0xFF || low == 0xFF)return false;   
    value.bytes[i] = (high << 4) | low;
  }
  
  if(type == RBLabSensCommand::ASK_CONC){
    return DataHandler::instance().setConcentration(channel, value.f);
  }
  if(type == RBLabSensCommand::ASK_VOLT){
    return DataHandler::instance().setVoltage(channel, value.f);
  }
  else 
    return false;
}

/***
* if responce from the sensor is [30 32 38 30] then zero is set
* if [30 31 30 30] or other, then  zero is not set
***/

bool RBLabSensParser::setZero(const volatile uint8_t* buffer){
  
  if (!buffer) return false;
  
  bool zeroSet = (buffer[1] == '2') ? true : false;
  
  return zeroSet; 
}


uint8_t RBLabSensParser::hexCharToNibble(uint8_t c){
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0xFF; 
}



void RBLabSensParser::answer(uint8_t*& buffer, uint16_t& length){

  RBLabSensCommand::Type command;
  command = static_cast<RBLabSensCommand::Type>(buffer[0]);
  
  switch(command) {
    case RBLabSensCommand::ASK_SUBST_NAME:
      response_len = sizeof(ask_subst_name);
      memcpy(response_buffer, ask_subst_name, response_len);
      break;
      
    case RBLabSensCommand::ASK_CONC:
    case RBLabSensCommand::ASK_VOLT: 
      response_len = sizeof(ask_conc_volt);
      memcpy(response_buffer, ask_conc_volt, response_len);
      break;
    
    case RBLabSensCommand::ZERO_SET: 
      response_len = sizeof(zero_set);
      memcpy(response_buffer, zero_set, response_len);
      break;
      
    case RBLabSensCommand::ASK_ZERO_END: 
      response_len = sizeof(ask_zero_set_end);
      memcpy(response_buffer, ask_zero_set_end, response_len);
      break;

    default:
      response_len = 0;
      
      
  }
  
  buffer = response_buffer;
  length = response_len;
}
