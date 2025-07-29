// RBLabParser.cpp

#include "RBLabParser.h"

//char debug_name[MAX_NAME_LENGTH] = {0};
//Config::SensorSettings debugSettings;
//uint8_t debug_config = 0;
uint8_t debug_channel = 0;

static const uint8_t PATTERN_TO_RBLAB[]  = {0x05, 0x37, 0x66, 0x34, 0x33, 0x00};
static const uint8_t PATTERN_ASK_PTH[]   = {0x3b, 0x66, 0x65, 0x39, 0x33, 0x00};
static const uint8_t PATTERN_ASK_PARAM[] = {0x3d, 0x37, 0x65, 0x39, 0x31, 0x00};
static const uint8_t PATTERN_READ_CONF[] = {0xd1, 0x37, 0x66, 0x31, 0x63, 0x00};
static const uint8_t PATTERN_READ_CONC[] = {0x49, 0x37, 0x65, 0x62, 0x36, 0x00};
static const uint8_t PATTERN_READ_VOLT[] = {0x47, 0x66, 0x66, 0x37, 0x32, 0x00};

static const RBLabParser::CommandEntry commandTable[] = {
    {RBLabCommand::TO_RBLAB_MODE,       PATTERN_TO_RBLAB,  true},
    {RBLabCommand::ASK_PTH,             PATTERN_ASK_PTH,   false},
    {RBLabCommand::ASK_PARAM,           PATTERN_ASK_PARAM, false},
    {RBLabCommand::READ_CONFIG,         PATTERN_READ_CONF, false},
    {RBLabCommand::READ_CONC,           PATTERN_READ_CONC, false},  
    {RBLabCommand::READ_VOLT,           PATTERN_READ_VOLT, false}, 

    // Dynamic parsers (pattern = NULL)
    {RBLabCommand::FROM_RBLAB_MODE,     0,                 false},
    {RBLabCommand::FROM_TRANS_MODE,     0,                 false},
    {RBLabCommand::TO_TRANS_MODE,       0,                 false},
    {RBLabCommand::WRITE_PARAM,         0,                 false},
    {RBLabCommand::WRITE_CONFIG,        0,                 false},
    {RBLabCommand::WRITE_SENSOR_CONFIG, 0,                 false},
    {RBLabCommand::READ_SENSOR_CONFIG,  0,                 false},
    {RBLabCommand::SET_TIME,            0,                 false},
};


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
  read_channel(MAX_SENSORS),
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

  if(!length || buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;
  
  for (int i = 0; i < sizeof(commandTable) / sizeof(commandTable[0]); ++i) {
    const CommandEntry& entry = commandTable[i];

    if (entry.pattern) {
      parse_error_code res = fixedSizeCommand(buffer, length, entry.pattern);
      if (res == PARSE_OK) {
          commandFromPC = entry.cmd;
          if (entry.enterMode) { enterRbLabMode(); }
          return PARSE_OK;
      }
    } else if (buffer[0] == entry.cmd) {
      return callSpecificParser(entry.cmd, buffer, length);
    }
  }
  return PARSE_UNKNOWN_MESSAGE;                                       
}

parse_error_code RBLabParser::callSpecificParser
(uint8_t cmd, const volatile uint8_t* buffer,uint16_t length) {
    switch (cmd) {
        case RBLabCommand::FROM_RBLAB_MODE:      
          return fromRbLabModeParse(buffer, length);
          
        case RBLabCommand::FROM_TRANS_MODE:      
          return fromTransModeParse(buffer, length);
          
        case RBLabCommand::TO_TRANS_MODE:        
          return toTransModeParse(buffer, length);
          
        case RBLabCommand::WRITE_PARAM:          
          return writeParamParse(buffer, length);
          
        case RBLabCommand::WRITE_CONFIG:         
          return writeConfigParse(buffer, length);
          
        case RBLabCommand::WRITE_SENSOR_CONFIG:  
          return writeSensorConfigParse(buffer, length);
          
        case RBLabCommand::READ_SENSOR_CONFIG:  
          return readSensorConfigParse(buffer, length);
          
        case RBLabCommand::SET_TIME:             
          return setTime(buffer, length);
    }
    return PARSE_UNKNOWN_MESSAGE;
}

parse_error_code RBLabParser::fixedSizeCommand (const volatile uint8_t* buffer,
                                                uint16_t length, 
                                                const uint8_t* validPattern) {
  const size_t expectedLen = sizeof(PATTERN_TO_RBLAB);
  
  if (length != expectedLen) { return PARSE_INVALID_LENGTH; }
  return (memcmp(const_cast<const uint8_t*>(buffer), 
                 validPattern, 
                 expectedLen) == 0) 
          ? PARSE_OK : PARSE_INVALID_FORMAT;
}

//PC-> 3c .......CRC 0   
parse_error_code RBLabParser::writeParamParse
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

parse_error_code RBLabParser::writeConfigParse
(const volatile uint8_t* buffer, uint16_t length){
  if(length > 7)return PARSE_INVALID_LENGTH;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
  /******************* parse alarm type *****************/
    if(buffer[1] == '1')
      Config::instance().
        writeAlarmType(Config::CommonSettings::ONE_THRESHOLD);
    else if((buffer[1] == '2'))
      Config::instance().
        writeAlarmType(Config::CommonSettings::TWO_THRESHOLD);
    else if((buffer[1] == '3'))
      Config::instance().
        writeAlarmType(Config::CommonSettings::INDIVIDUAL_RELAY);
    commandFromPC = RBLabCommand::WRITE_CONFIG;
    return PARSE_OK;
  }
  else return PARSE_CRC_ERROR;
}

//PC-> d1 .......CRC 0   
parse_error_code RBLabParser::writeSensorConfigParse
(const volatile uint8_t* buffer, uint16_t length){
  Config::SensorSettings sensorSettings;
  uint8_t pointer =sizeof(uint8_t);
  uint8_t channel = 0;
  
  if(length > 88)return PARSE_INVALID_LENGTH;
  if(buffer[length - 1] != 0) return PARSE_INVALID_FORMAT;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){

  /******************* parse channel number *****************/
    
    if((buffer[pointer] - '0') > 2) return PARSE_INVALID_FORMAT;
    channel = buffer[pointer] - '0';
    pointer++;
    
  /**************** parse substance name *******************/
    int i = 0;
    for(; i < MAX_NAME_LENGTH ; ++i){
      sensorSettings.name[i] = buffer[pointer + i];
      if(sensorSettings.name[i] == 0x0a){
        i++;
        break;
      }
    }
    sensorSettings.name[i-1] = '\0';
    pointer += i; 
    
  /***************** parse substance formula ******************/
    i = 0;
    for(; i < MAX_FORMULA_LENGTH ; ++i){
      sensorSettings.formula[i] = buffer[pointer + i];
      if(sensorSettings.formula[i] == 0x0a){
        i++;
        break;
      }
    }
    sensorSettings.formula[i-1] = '\0';
    pointer += i; 
    
    /***************** parse start range ******************/
    float val = 0;
    if(asciiToFloat(&buffer[pointer], val)) sensorSettings.start_range = val;
    else return PARSE_INVALID_FORMAT;
    pointer += ASCII_FLOAT_SIZE;
    
    /***************** parse end range ******************/
    
    if(asciiToFloat(&buffer[pointer], val)) sensorSettings.end_range = val;
    else return PARSE_INVALID_FORMAT;
    pointer += ASCII_FLOAT_SIZE;
    
    /***************** parse threshold1 ******************/
    
    if(asciiToFloat(&buffer[pointer], val)) sensorSettings.threshold_1 = val;
    else return PARSE_INVALID_FORMAT;
    pointer += ASCII_FLOAT_SIZE;
    
    /***************** parse direction1 ******************/

    if(buffer[pointer] == '0')
      sensorSettings.alarm1Direction = Config::SensorSettings::UP;
    else if(buffer[pointer] == '1')
      sensorSettings.alarm1Direction = Config::SensorSettings::DOWN;
    else return PARSE_INVALID_FORMAT;
    pointer += sizeof(uint8_t);
    
    /***************** parse threshold2 ******************/
    
    if(asciiToFloat(&buffer[pointer], val)) sensorSettings.threshold_2 = val;
    else return PARSE_INVALID_FORMAT;
    pointer += ASCII_FLOAT_SIZE;
    
    /***************** parse direction1 ******************/

    if(buffer[pointer] == '0')
      sensorSettings.alarm2Direction = Config::SensorSettings::UP;
    else if(buffer[pointer] == '1')
      sensorSettings.alarm2Direction = Config::SensorSettings::DOWN;
    else return PARSE_INVALID_FORMAT;
    pointer++;
    
    /***************** parse units ******************/
    
    if(buffer[pointer] == '0')
      sensorSettings.units = Config::SensorSettings::MG_PER_M3;
    else if(buffer[pointer] == '1')
      sensorSettings.units = Config::SensorSettings::PERCENT;
    else return PARSE_INVALID_FORMAT;
    pointer++;
    
    /***************** parse decimal ******************/
    
    if((buffer[pointer] - '0') > 7) return PARSE_INVALID_FORMAT;
    sensorSettings.decimal = buffer[pointer] - '0';
    
    Config::instance().writeSensorSettings(channel, sensorSettings);
     
    commandFromPC = RBLabCommand::WRITE_SENSOR_CONFIG;
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

parse_error_code RBLabParser::readSensorConfigParse
(const volatile uint8_t* buffer, uint16_t length){
  if(length > 7)return PARSE_INVALID_LENGTH;
  if(crc16(buffer, length - 5) == asciiHexToCrc(&buffer[length - 5])){
    
  /******************* parse channel number *****************/
    
    if((buffer[1] - '0') > 2) return PARSE_INVALID_FORMAT;
    read_channel = buffer[1] - '0';
    commandFromPC = RBLabCommand::READ_SENSOR_CONFIG;
    return PARSE_OK;
  }
  else return PARSE_CRC_ERROR;
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


//PC -> 04 00 + some message
parse_error_code RBLabParser::fromTransModeParse
(const volatile uint8_t* buffer, uint16_t length){
  
  if (length < 2) { return PARSE_INVALID_LENGTH; }
  
  if(buffer[1] == 0x00) { exitTransparentMode();}
  
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
  if (length < 3) { return PARSE_INVALID_LENGTH; }
  if (length == 3 && (buffer[1] & 0xF8) == 0x30 && buffer[2] == 0x00) {
    uint8_t n = buffer[1] & 0x07;
    setTransChannel(n);
    commandFromPC = RBLabCommand::TO_TRANS_MODE;
    return PARSE_OK;
  }
  return PARSE_UNKNOWN_MESSAGE; 
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
    case RBLabCommand::WRITE_SENSOR_CONFIG:
    {
      response_len = sizeof(to_rblab_mode);
      memcpy(response_buffer, to_rblab_mode, response_len);
    }
    break;
    
    case RBLabCommand::READ_CONFIG:
    {
      response_len = 0;
      
      response_buffer[response_len] = RBLabCommand::READ_CONFIG;
      response_len ++;
      
      /****************** add alarm type *******************/
      
      switch (Config::instance().readAlarmType()){
        case Config::CommonSettings::ONE_THRESHOLD:
          response_buffer[response_len] = '1';
        break;
        case Config::CommonSettings::TWO_THRESHOLD:
          response_buffer[response_len] = '2';
        break;
        case Config::CommonSettings::INDIVIDUAL_RELAY:
          response_buffer[response_len] = '3';
        break;
      }
      response_len ++;
      /********************* add version *******************/
      
      uint8_t version[5];
      Config::instance().getVersion(version);
      memcpy(&response_buffer[response_len], version, sizeof(version));
      response_len += sizeof(version);
      
      /**************** add modbus address *****************/
      uint8_t modbusAddr[ASCII_INTEGER_SIZE];
      intToAscii(Config::instance().readModbusAddr(), modbusAddr);
      memcpy(&response_buffer[response_len], 
             modbusAddr, 2);  // send two least digits of address
      response_len += 2;
      
      addCrcAndNull(response_buffer, response_len);
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
      
      /********************* add version *******************/
    
      uint8_t version[5];
      Config::instance().getVersion(version);
      memcpy(&response_buffer[response_len], version, sizeof(version));
      response_len += sizeof(version);
      
      /**************** add sensors matrix *****************/
      
      memcpy(&response_buffer[response_len], 
             sensors_matrix, sizeof(sensors_matrix));
      response_len += sizeof(sensors_matrix);
      
      /****************** add type of PDK ******************/
      
      response_buffer[response_len] = '0' + Config::instance().readPDKType();
      response_len++;
      
      /****************** add alarm1 threshold *************/
      
      uint8_t PDKTreshold[ASCII_FLOAT_SIZE];
      floatToAscii(Config::instance().readAlarmThreshold_1(), PDKTreshold);
      memcpy(&response_buffer[response_len], PDKTreshold, sizeof(PDKTreshold));
      response_len += sizeof(PDKTreshold);
      
      /****************** add sensors queue ****************/
      
      memcpy(&response_buffer[response_len], 
             sensors_queue, sizeof(sensors_queue));
      response_len += sizeof(sensors_queue);
      
      /****************** add time of delay ****************/
      
      uint8_t noAlarmDelay[4];
      intToAscii(Config::instance().readBlockingTime(), noAlarmDelay);
      memcpy(&response_buffer[response_len], 
             noAlarmDelay, sizeof(noAlarmDelay));
      response_len += sizeof(noAlarmDelay);
      
      /******** add value of pressure compensation *********/
      
      uint8_t pressureCompensation[4];
      intToAscii(Config::instance().readPressureCompensation(), 
                 pressureCompensation);
      memcpy(&response_buffer[response_len], 
             pressureCompensation, sizeof(pressureCompensation));
      response_len += sizeof(pressureCompensation);
      
      /****************** add alarm2 threshold *************/
      
      floatToAscii(Config::instance().readAlarmThreshold_2(), PDKTreshold);
      memcpy(&response_buffer[response_len], PDKTreshold, sizeof(PDKTreshold));
      response_len += sizeof(PDKTreshold);

      addCrcAndNull(response_buffer, response_len);
    }
    break;
    case RBLabCommand::READ_SENSOR_CONFIG:
    {
      Config::SensorSettings sensorSettings;
      Config::instance().readSensorSettings(read_channel, sensorSettings);
      
      response_buffer[0] = RBLabCommand::READ_SENSOR_CONFIG;
      response_len = 1;
      
      /**************** add channel number *******************/
      response_buffer[response_len] = read_channel + '0';
      response_len++;
      
      /*********** add sensor name from device **************/
      uint8_t name_lemgth = strlen(sensorSettings.name);
      memcpy(&response_buffer[response_len], sensorSettings.name, name_lemgth);
      response_len += name_lemgth;
      response_buffer[response_len] = 0x0a;
      response_len++;
      
      /*********** add sensor name from sensor **************/
      const char* nameFromSensor = DataHandler::instance().
                                                  getName(read_channel);
      name_lemgth = strlen(nameFromSensor);
      if(name_lemgth < MAX_NAME_LENGTH){
        memcpy(&response_buffer[response_len], nameFromSensor, name_lemgth);
        response_len += name_lemgth;
        response_buffer[response_len] = 0x0a;
        response_len++;
      }
      /********************* add range start*****************/
      uint8_t floatValBuf[ASCII_FLOAT_SIZE];
      floatToAscii(sensorSettings.start_range, floatValBuf);
      memcpy(&response_buffer[response_len], floatValBuf, ASCII_FLOAT_SIZE);
      response_len += ASCII_FLOAT_SIZE;
      
      /********************* add range end*******************/
      floatToAscii(sensorSettings.end_range, floatValBuf);
      memcpy(&response_buffer[response_len], floatValBuf, ASCII_FLOAT_SIZE);
      response_len += ASCII_FLOAT_SIZE;
      
      /*************** add first threshold*******************/
      floatToAscii(sensorSettings.threshold_1, floatValBuf);
      memcpy(&response_buffer[response_len], floatValBuf, ASCII_FLOAT_SIZE);
      response_len += ASCII_FLOAT_SIZE;
      
      /*********** add first threshold direction ************/
      response_buffer[response_len] = 
        (sensorSettings.alarm1Direction == Config::SensorSettings::UP)?'0':'1';
      response_len++;
      
      /*************** add second threshold*******************/
      floatToAscii(sensorSettings.threshold_2, floatValBuf);
      memcpy(&response_buffer[response_len], floatValBuf, ASCII_FLOAT_SIZE);
      response_len += ASCII_FLOAT_SIZE;
      
      /*********** add second threshold direction ************/
      response_buffer[response_len] = 
        (sensorSettings.alarm2Direction == Config::SensorSettings::UP)? '0':'1';
      response_len++;
      
      /********************* add units **********************/
      response_buffer[response_len] = 
        (sensorSettings.units == Config::SensorSettings::MG_PER_M3) ? '0' : '1';
      response_len++;
      
      /********************* add decimal *********************/
      response_buffer[response_len] = '0'  + sensorSettings.decimal;
      response_len++;
        
      /************************* crc ************************/
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
  Channels::instance().set(trans_channel);
  if(stateMachine)stateMachine->setEvent(AppEvent::TRANSPARENT_MODE_REQUEST);
  Mediator::instance().setTransMode(); 
}

void RBLabParser::exitTransparentMode(){
  Mediator::instance().setNormalMode();
  if (stateMachine) stateMachine->setEvent(AppEvent::EXIT_TRANSPARENT_MODE);
}

void RBLabParser::enterRbLabMode(){
  Mediator::instance().setNormalMode();
  if (stateMachine) stateMachine->setEvent(AppEvent::RBLAB_MODE_REQUEST);
}

void RBLabParser::exitRbLabMode(){
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

