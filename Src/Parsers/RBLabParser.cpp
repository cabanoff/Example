// RBLabParser.cpp

#include "RBLabParser.h"

uint8_t exit_trans_mode = 0;
uint8_t enter_trans_mode = 0;
uint8_t deb_channel = 0;
uint8_t deb_len = 0;
uint8_t deb_name[MAX_NAME_LENGTH] = {0};


uint8_t RBLabParser::response_buffer[] = {0}; 

static const uint8_t to_rblab_mode[] = 
  {0x06, 0x33, 0x66, 0x34, 0x32, 0x00};
static const uint8_t open_channel_mess[] = 
  {0x32,0x30,0x30};
static const uint8_t pth_answer_stub[] = 
  {0x3b, 0x39, 0x61, 0x38, 0x37, 0x32, 0x37, 0x34, 0x34, 0x39, 0x61, 0x38, 0x37,
   0x32, 0x37, 0x34, 0x34, 0x39, 0x61, 0x38, 0x37, 0x32, 0x37, 0x34, 0x34, 0x65,
   0x37, 0x64, 0x31, 0x00};
static const uint8_t no_zero_set[] = 
  {0x30, 0x30, 0x30, 0x30};
static const uint8_t sensors_matrix[] = 
  {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
static const uint8_t sensors_queue[] = 
  {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};




RBLabParser::RBLabParser():
  trans_channel(MAX_SENSORS),
  commandFromPC(RBLabCommand::NONE),
  commandToMCU(RBLabCommand::NONE)
  {
    stateMachine = NULL;
  }


void RBLabParser::setTransChannel(uint8_t ch_index){
  trans_channel = ch_index;
}

parse_error_code RBLabParser::parseFrame(const volatile uint8_t* buffer, 
                                         uint16_t length){
  //if got `:001904123408\r\n`, ignore it  
  if(!length) return PARSE_INVALID_FORMAT;
  if(buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;
  
  
  switch(buffer[0]){
    case RBLabCommand::TO_RBLAB_MODE:
      return toRBLadbModeParse(buffer, length);
      
    case RBLabCommand::FROM_RBLAB_MODE:
      return fromRbLabModeParse(buffer, length);
      
    case RBLabCommand::FROM_TRANS_MODE:
      return fromTransModeParse(buffer, length);
      
    case RBLabCommand::TO_TRANS_MODE:  
      return toTransModeParse(buffer, length);
      
    case RBLabCommand::ASK_PTH:  
      return pthAskParse(buffer, length);
    
    case RBLabCommand::ASK_PARAM:  
      return paramAskParse(buffer, length);
    
    case RBLabCommand::WRITE_PARAM:  
      return writeParam(buffer, length);
      
    case RBLabCommand::WRITE_CONFIG:  
      return writeConfig(buffer, length);
      
    case RBLabCommand::READ_CONC: 
      return readConcentration(buffer, length);
    
    case RBLabCommand::READ_VOLT:  
      return readVoltage(buffer, length);
    
    case RBLabCommand::SET_TIME:  
      return setTime(buffer, length);
  }
  return PARSE_UNKNOWN_MESSAGE;                                       
}

//PC-> 3c .......CRC 0   
parse_error_code RBLabParser::writeParam
(const volatile uint8_t* buffer, uint16_t length){
  if(length != 47)return PARSE_INVALID_LENGTH;
  if(buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
    bool isValid = true;
    int16_t blockingTime = asciiToInt(&buffer[26], isValid);
    if(isValid){
      Config::instance().writeBlockingTime(blockingTime);
      commandFromPC = RBLabCommand::WRITE_PARAM;
      return PARSE_OK;
    }
    return PARSE_INVALID_FORMAT;
  }
  else return PARSE_CRC_ERROR;
}

//PC-> D0 .......CRC 0   
parse_error_code RBLabParser::writeConfig
(const volatile uint8_t* buffer, uint16_t length){
  uint8_t pointer = 0;
  if(length < 5)return PARSE_INVALID_LENGTH;
  if(buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
    pointer++;
    uint8_t channel = 0;
    if((buffer[pointer] - '0') > '2') return PARSE_INVALID_FORMAT;
    channel = buffer[pointer] - '0';
    pointer++;
    int i = 0;
    for(; i < MAX_NAME_LENGTH ; ++i){
      deb_name[i] = buffer[pointer + i];
      if(buffer[pointer + i] == 0x0a)break;
    }
    deb_channel = channel;
    deb_len = i - 1;
    DataHandler::instance().setName(channel,&buffer[pointer],i - 1);
    pointer += i; 
    commandFromPC = RBLabCommand::WRITE_CONFIG;
    return PARSE_OK;
  }
  else return PARSE_CRC_ERROR;
}



// PC -> 33 Y Y 20 M M 20 D D 20 h h 20 m m 20 s s crc crc crc crc /0
parse_error_code RBLabParser::setTime
(const volatile uint8_t* buffer, uint16_t length){
  if(length != 23) return PARSE_INVALID_LENGTH;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
    uint8_t date_time[17];
    for (int i = 0; i < sizeof(date_time); ++i) {
      date_time[i] = buffer[i+1];
    }
    commandFromPC = RBLabCommand::SET_TIME;
    RtcTask::instance().setDateTime(date_time);
    return PARSE_OK;
  }
  return PARSE_CRC_ERROR;
}

// PC -> 19 00
parse_error_code RBLabParser::fromRbLabModeParse
(const volatile uint8_t* buffer, uint16_t length){
  if (length < 2) return PARSE_INVALID_LENGTH;
  
  if(buffer[1] == 0x00){
    commandFromPC = RBLabCommand::NONE;
    exitRbLabMode();
    return PARSE_OK;
  }

  return PARSE_UNKNOWN_MESSAGE;

}

  // PC -> 05 37 66 34 33 00
  // 06 33 66 34 32 00 -> PC
parse_error_code RBLabParser::toRBLadbModeParse
(const volatile uint8_t* buffer, uint16_t length){
  
  if(length != 6)return PARSE_INVALID_LENGTH;
  uint8_t validMessage[] = {0x05, 0x37, 0x66, 0x34, 0x33, 0x00};
  if( memcmp(const_cast<const uint8_t*>(buffer), 
             validMessage, 
             sizeof(validMessage)) == 0){
    commandFromPC = RBLabCommand::TO_RBLAB_MODE;
    return PARSE_OK;
  }
  return PARSE_INVALID_FORMAT;
}

//PC -> 04 00 + some message
parse_error_code RBLabParser::fromTransModeParse
(const volatile uint8_t* buffer, uint16_t length){
  
  if (length < 2) return PARSE_INVALID_LENGTH;
  
  if(buffer[1] == 0x00){
    Mediator::instance().setNormalMode();
  }
  
  if (length == 2) {
    commandFromPC = RBLabCommand::NONE;
    return PARSE_OK;
  }
  return parseFrame(&buffer[2], length - 2); 
}

//PC -> 32 3x 00
parse_error_code RBLabParser::toTransModeParse
(const volatile uint8_t* buffer, uint16_t length){
  // Match pattern: [0x32, 0x3n, 00]
  if (length < 3) return PARSE_INVALID_LENGTH;
  if (length == 3 && (buffer[1] & 0xF8) == 0x30 && buffer[2] == 0x00) {
    uint8_t n = buffer[1] & 0x07;
    setTransChannel(n);
    commandFromPC = RBLabCommand::TO_TRANS_MODE;
    return PARSE_OK;
  }
  return PARSE_UNKNOWN_MESSAGE; 
}

// PC -> 3b 66 65 39 33 00
parse_error_code RBLabParser::pthAskParse
(const volatile uint8_t* buffer, uint16_t length){
  if(length != 6)return PARSE_INVALID_LENGTH;
  uint8_t validMessage[] = {0x3b, 0x66, 0x65, 0x39, 0x33, 0x00};
  if( memcmp(const_cast<const uint8_t*>(buffer), 
             validMessage, 
             sizeof(validMessage)) == 0){
    commandFromPC = RBLabCommand::ASK_PTH;
    return PARSE_OK;
  }
  return PARSE_INVALID_FORMAT;
}

// PC -> 3d 37 65 39 31 00
parse_error_code RBLabParser::paramAskParse
(const volatile uint8_t* buffer, uint16_t length){
  if(length != 6)return PARSE_INVALID_LENGTH;
  uint8_t validMessage[] = {0x3d, 0x37, 0x65, 0x39, 0x31, 0x00};
  if( memcmp(const_cast<const uint8_t*>(buffer), 
             validMessage, 
             sizeof(validMessage)) == 0){
    commandFromPC = RBLabCommand::ASK_PARAM;
    return PARSE_OK;
  }
  return PARSE_INVALID_FORMAT;
}

  // PC -> 47 66 66 37 32 00
parse_error_code RBLabParser::readVoltage
(const volatile uint8_t* buffer, uint16_t length){
  
  if(length != 6)return PARSE_INVALID_LENGTH;
  uint8_t validMessage[] = {0x47, 0x66, 0x66, 0x37, 0x32, 0x00};
  if( memcmp(const_cast<const uint8_t*>(buffer), 
             validMessage, 
             sizeof(validMessage)) == 0){
    commandFromPC = RBLabCommand::READ_VOLT;
    exitRbLabMode();
    return PARSE_OK;
  }
  return PARSE_INVALID_FORMAT;
}

  // PC -> 49 37 65 62 36 00
parse_error_code RBLabParser::readConcentration
(const volatile uint8_t* buffer, uint16_t length){
  
  if(length != 6)return PARSE_INVALID_LENGTH;
  uint8_t validMessage[] = {0x49, 0x37, 0x65, 0x62, 0x36, 0x00};
  if( memcmp(const_cast<const uint8_t*>(buffer), 
             validMessage, 
             sizeof(validMessage)) == 0){
    commandFromPC = RBLabCommand::READ_CONC;
    exitRbLabMode();
    return PARSE_OK;
  }
  return PARSE_INVALID_FORMAT;
}

RBLabCommand::Type RBLabParser::readCommandToMCU(){
  RBLabCommand::Type command = commandToMCU;
  commandToMCU = RBLabCommand::NONE;
  return command;
}

void RBLabParser::answer(uint8_t*& buffer, uint16_t& length){

  RBLabCommand::Type command = commandFromPC;
    
  commandFromPC = RBLabCommand::NONE;
    
  switch(command) {
    case RBLabCommand::TO_RBLAB_MODE:
    case RBLabCommand::SET_TIME:
    case RBLabCommand::WRITE_PARAM:
    case RBLabCommand::WRITE_CONFIG:
    {
      response_len = sizeof(to_rblab_mode);
      memcpy(response_buffer, to_rblab_mode, response_len);
    }
    break;
      
    case RBLabCommand::TO_TRANS_MODE:
    {
      enterTransMode(trans_channel);
      response_len = sizeof(open_channel_mess);
      memcpy(response_buffer, open_channel_mess, response_len);
      response_buffer[2] |= trans_channel;
      addCrcAndNull(response_buffer, response_len);
 
    }
    break;
      
    case RBLabCommand::ASK_PTH:
      response_len = sizeof(pth_answer_stub);
      memcpy(response_buffer, pth_answer_stub, response_len);
    break;
      
    case RBLabCommand::READ_CONC:
    {  
      response_buffer[0] = RBLabCommand::READ_CONC;
      uint8_t conc_buffer[ASCII_FLOAT_SIZE];
      response_len = 1;
      for(int i = 0; i < MAX_SENSORS; i++){
        float val = DataHandler::instance().getConcentration(i);
        floatToAscii(val, conc_buffer);
        memcpy(&response_buffer[response_len], conc_buffer, ASCII_FLOAT_SIZE);
        response_len += ASCII_FLOAT_SIZE;
      }
      addCrcAndNull(response_buffer, response_len);

    }
    break;
    case RBLabCommand::READ_VOLT:
    {
      response_buffer[0] = RBLabCommand::READ_VOLT;
      uint8_t volt_buffer[ASCII_FLOAT_SIZE];
      uint8_t work_mode[4];
      response_len = 1;
      for(int i = 0; i < MAX_SENSORS; i++){
        float val = DataHandler::instance().getVoltage(i);
        floatToAscii(val, volt_buffer);
        memcpy(&response_buffer[response_len], volt_buffer, ASCII_FLOAT_SIZE);
        response_len += ASCII_FLOAT_SIZE;
        memcpy(work_mode,no_zero_set,sizeof(work_mode));
        memcpy(&response_buffer[response_len], work_mode, sizeof(work_mode));
        response_len += sizeof(work_mode);
      }
      addCrcAndNull(response_buffer, response_len);

    }
    break;
    case RBLabCommand::ASK_PARAM:
    {
      response_buffer[0] = RBLabCommand::ASK_PARAM;
      response_len = 1;
      uint8_t version[5];
      Config::instance().getVersion(version);
      memcpy(&response_buffer[response_len], version, sizeof(version));
      response_len += sizeof(version);
      
      memcpy(&response_buffer[response_len], 
             sensors_matrix, sizeof(sensors_matrix));
      response_len += sizeof(sensors_matrix);
      
      response_buffer[response_len] = '0' + Config::instance().readPDKType();
      response_len++;
      
      uint8_t PDKTreshold[8];
      floatToAscii(Config::instance().readAlarmThreshold_1(), PDKTreshold);
      memcpy(&response_buffer[response_len], PDKTreshold, sizeof(PDKTreshold));
      response_len += sizeof(PDKTreshold);
      
      memcpy(&response_buffer[response_len], 
             sensors_queue, sizeof(sensors_queue));
      response_len += sizeof(sensors_queue);
      
      uint8_t noAlarmDelay[4];
      intToAscii(Config::instance().readBlockingTime(), noAlarmDelay);
      memcpy(&response_buffer[response_len], 
             noAlarmDelay, sizeof(noAlarmDelay));
      response_len += sizeof(noAlarmDelay);
      
      uint8_t pressureCompensation[4];
      intToAscii(Config::instance().readPressureCompensation(), 
                 pressureCompensation);
      memcpy(&response_buffer[response_len], 
             pressureCompensation, sizeof(pressureCompensation));
      response_len += sizeof(pressureCompensation);
      
      floatToAscii(Config::instance().readAlarmThreshold_2(), PDKTreshold);
      memcpy(&response_buffer[response_len], PDKTreshold, sizeof(PDKTreshold));
      response_len += sizeof(PDKTreshold);

      addCrcAndNull(response_buffer, response_len);
    }
    break;
    default:
      response_len = 0;     
  }
  
  buffer = response_buffer;
  length = response_len;
}

void RBLabParser::addCrcAndNull(uint8_t* buffer, uint16_t& len){
  uint8_t crc_buffer[4];
  crcToAscii(buffer, len, crc_buffer);
  memcpy(&buffer[len], crc_buffer, sizeof(crc_buffer));
  len += sizeof(crc_buffer);
  buffer[len] = '\0';
  len += 1;
}

void RBLabParser::enterTransMode(uint8_t trans_channel){
  enter_trans_mode++;
  Channels::instance().set(trans_channel);
  if(stateMachine)stateMachine->setEvent(AppEvent::RBLAB_MODE_REQUEST);
  Mediator::instance().setTransMode(); 
}

void RBLabParser::exitRbLabMode(){
  exit_trans_mode++;
  Mediator::instance().setNormalMode();
  if (stateMachine) stateMachine->setEvent(AppEvent::EXIT_RBLAB_MODE);
}

void RBLabParser::crcToAscii(uint8_t* source, uint16_t length, 
                            uint8_t* crc_buffer){
  uint16_t crc = crc16(const_cast<const volatile uint8_t*>(source), length);
  // Convert each nibble of the CRC to a lowercase ASCII hex character
  // High byte first, then low byte
  crc_buffer[2] = "0123456789abcdef"[(crc >> 12) & 0x0F];
  crc_buffer[3] = "0123456789abcdef"[(crc >> 8) & 0x0F];
  crc_buffer[0] = "0123456789abcdef"[(crc >> 4) & 0x0F];
  crc_buffer[1] = "0123456789abcdef"[crc & 0x0F];
  
}

void RBLabParser::floatToAscii(float val, uint8_t* conc_buffer) {
    union {
        float f_val;
        uint8_t bytes[4];
    } converter;
    
    converter.f_val = val;
    
    for (int i = 0; i < 4; i++) {
        uint8_t byte = converter.bytes[i];
        uint8_t high_nibble = (byte >> 4) & 0x0F;
        uint8_t low_nibble = byte & 0x0F;
        
        conc_buffer[2*i] = (high_nibble < 10) ? 
                           ('0' + high_nibble) : 
                           ('a' + high_nibble - 10);
        conc_buffer[2*i+1] = (low_nibble < 10) ? 
                             ('0' + low_nibble) : 
                             ('a' + low_nibble - 10);
    }
}

void RBLabParser::intToAscii(int16_t val, uint8_t* conc_buffer) {
  union {
    int16_t i;
    uint8_t b[2]; // little-endian: b[0] = LSB, b[1] = MSB
  } u;
  u.i = val;

  for (int i = 0; i < 2; ++i) {
    uint8_t byte = u.b[i];
    conc_buffer[i * 2 + 0] = 
      (byte >> 4) < 10 ? ('0' + (byte >> 4)) : ('a' + ((byte >> 4) - 10));
    conc_buffer[i * 2 + 1] = 
      (byte & 0x0F) < 10 ? ('0' + (byte & 0x0F)) : ('a' + ((byte & 0x0F) - 10));
  }
}

int16_t RBLabParser::asciiToInt(const volatile uint8_t buffer[4], bool& valid){
  uint8_t nibbles[4];
  valid = true;
  for (int i = 0; i < 4; ++i) {
      if (buffer[i] >= '0' && buffer[i] <= '9') {
          nibbles[i] = buffer[i] - '0';
      } else if (buffer[i] >= 'A' && buffer[i] <= 'F') {
          nibbles[i] = buffer[i] - 'A' + 10;
      } else if (buffer[i] >= 'a' && buffer[i] <= 'f') {
          nibbles[i] = buffer[i] - 'a' + 10;
      } else {
          valid = false;
          return 0;
      }
  }

  uint8_t low_byte = (nibbles[0] << 4) | nibbles[1];
  uint8_t high_byte = (nibbles[2] << 4) | nibbles[3];
  uint16_t combined = (static_cast<uint16_t>(high_byte) << 8) | low_byte;
  return static_cast<int16_t>(combined);
}

