//Config.cpp

#include "Config.h"

//Config::Settings debug_s = {0};

const Config::CommonSettings Config::defaultCommonSettings = {
   2,           //modbusAddr
   60,         //blockingTime
   -10,         //pressureCompensation 
   50,           //pumpPower duty cycle
   1.0,         //alarmThreshold_1;
   2.0,         //alarmThreshold_2;
   Config::CommonSettings::TWO_THRESHOLD,
   Config::CommonSettings::PDK_MAX_ALLOWED
};

const Config::CommonSettings Config::minCommonSettings = {
   1,   
   30,
   -50,  
   0,
   0.0,    
   1.0,
   Config::CommonSettings::TWO_THRESHOLD,
   Config::CommonSettings::PDK_MAX_ALLOWED
};

const Config::CommonSettings Config::maxCommonSettings = {
   255,        //modbusAddr
   900,        //blockingTime
   50,         //pressureCompensation  
   100,
   100.0,      //alarmThreshold_1;  
   30.0,       //alarmThreshold_2;
   Config::CommonSettings::TWO_THRESHOLD,
   Config::CommonSettings::PDK_WORK_ZONE
};





const Config::SensorSettings Config::defaultSensorSettings = {
   "None",
   "H2O",
    0.0,     // start_range;  
    20.0,     // end_range; 
    2.0,     // threshold_1;
    Config::SensorSettings::DOWN,
    1.0,     // threshold_2;
    Config::SensorSettings::DOWN,
    Config::SensorSettings::MG_PER_M3, //units
    3,  //decimal points
};

const char Config::MG_PER_M3[] = 
{0x28, 0xEC, 0xA3, 0x2F, 0xEC, 0x33, 0x29, 0x00}; //"(mg/m3)";
const char Config::PERCENT[] = {0x28, 0x25, 0x29, 0x00};//"(%)";
const char Config::MV[] = {0x28, 0xEC, 0x82, 0x29, 0x00};//"(mV)";
const char Config::ERROR[] = {0x28, 0x2d, 0x29, 0x00};//"(-)";

STATIC_ASSERT(sizeof(VERSION) == 5, version_string_must_be_4_characters);

  
void Config::init(){
  Settings s;
  readSettings(s);

  if(s.check_sum != calculateChecksum(s)){
    s.common = defaultCommonSettings;
    for (uint8_t i = 0; i < MAX_SENSORS_CONF; ++i) {
        s.sensor[i] = defaultSensorSettings;
    }
    writeSettings(s);
  }
  
//  readSettings(debug_s);
}


void Config::getVersion(uint8_t version[5]) const{
    const char* v = VERSION;
    // Copy ASCII values of each character
    for (int i = 0; i < 4; i++) {
        version[i] = static_cast<uint8_t>(v[i]);
    }
    // Add newline terminator (0x0A)
    version[4] = 0x0a;
}

uint16_t Config::readModbusAddr() const {
    Settings s;
    readSettings(s);
    return s.common.modbusAddr;
}

void Config::writeModbusAddr(uint16_t addr) {
    Settings s;
    readSettings(s);
    if((addr >= minCommonSettings.modbusAddr) && 
       (addr <= maxCommonSettings.modbusAddr)){
         
        s.common.modbusAddr = addr;
        writeSettings(s);
     }

}

int16_t Config::readBlockingTime() const {
    Settings s;
    readSettings(s);
    return (s.common.blockingTime > 0) ? s.common.blockingTime : 0;
}

void Config::writeBlockingTime(int16_t time) {
    Settings s;
    readSettings(s);
    if((time >= minCommonSettings.blockingTime) && 
       (time <= maxCommonSettings.blockingTime)){
        s.common.blockingTime = time;
        writeSettings(s);
     }
}


int16_t Config::readPressureCompensation() const {
    Settings s;
    readSettings(s);
    return s.common.pressureCompensation;
}

void Config::writePressureCompensation(int16_t val) {
    Settings s;
    readSettings(s);
    if((val >= minCommonSettings.pressureCompensation) && 
       (val <= maxCommonSettings.pressureCompensation)){
        s.common.pressureCompensation = val;
        writeSettings(s);
    }
}

float Config::readAlarmThreshold_1() const {
    Settings s;
    readSettings(s);
    return s.common.alarmThreshold_1;
}

void Config::writeAlarmThreshold_1(float threshold) {
    Settings s;
    readSettings(s);
    s.common.alarmThreshold_1 = threshold;
    writeSettings(s);
}

float Config::readAlarmThreshold_2() const {
    Settings s;
    readSettings(s);
    return s.common.alarmThreshold_2;
}

void Config::writeAlarmThreshold_2(float threshold) {
    Settings s;
    readSettings(s);
    s.common.alarmThreshold_2 = threshold;
    writeSettings(s);
}

float Config::readStartRange(uint8_t channel) const {
    float val = 0.0;
    if(channel < MAX_SENSORS_CONF){
      Settings s;
      readSettings(s);
      val =  s.sensor[channel].start_range;
    }
    return val;
}

void Config::writeStartRange(uint8_t channel, float start_range) {
    if(channel < MAX_SENSORS_CONF){
      Settings s;
      readSettings(s);
      s.sensor[channel].start_range = start_range;
      writeSettings(s);
    }
}

float Config::readThreshold_1(uint8_t channel) const {
  float val = 0.0;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    val =  s.sensor[channel].threshold_1;
  }
  return val;
}

float Config::readThreshold_2(uint8_t channel) const {
  float val = 0.0;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    val = s.sensor[channel].threshold_2;
  }
  return val;
}

Config::SensorSettings::AlarmDirection Config::readThreshold1Direction
  (uint8_t channel)
{
  SensorSettings::AlarmDirection val = SensorSettings::UP;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    val = s.sensor[channel].alarm1Direction;
  }
  return val;
}

Config::SensorSettings::AlarmDirection Config::readThreshold2Direction
  (uint8_t channel)
{
  SensorSettings::AlarmDirection val = SensorSettings::UP;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    val = s.sensor[channel].alarm2Direction;
  }
  return val;
}

Config::CommonSettings::PDKType Config::readPDKType(){
    Settings s;
    readSettings(s);
    return s.common.pdkType; 
}

void Config::writePDKType(CommonSettings::PDKType type){
    Settings s;
    readSettings(s);
    s.common.pdkType = type;
    writeSettings(s);
}

void Config::readSensorName
  ( uint8_t channel, char* outName, size_t maxLen) const {
    
  if((maxLen == 0)||(maxLen > MAX_NAME_LENGTH)) return;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    if(strcmp(s.sensor[channel].name,"None") == 0)
      strncpy(outName, 
              DataHandler::instance().getName(channel),
              maxLen);
    else
      strncpy(outName, s.sensor[channel].name, maxLen);
    outName[maxLen - 1] = '\0'; // null-terminate
  }
  else{
    strncpy(outName, "None", maxLen);
    outName[maxLen - 1] = '\0'; // null-terminate
  }
}

void Config::readSensorSettings
   (uint8_t channel, SensorSettings& sensorSettings) const {
    
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    sensorSettings = s.sensor[channel];
  }
}

void Config::writeSensorName(uint8_t channel, const char* newName) {
 
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    memset(s.sensor[channel].name, 0, sizeof(s.sensor[channel].name));
    strncpy(s.sensor[channel].name, newName, sizeof(s.sensor[channel].name));
    s.sensor[channel].name[sizeof(s.sensor[channel].name) - 1] = '\0';
    writeSettings(s);
  }
}

void Config::readSensorFormula
  ( uint8_t channel, char* outFormula, size_t maxLen) const {
    
  if(maxLen == 0) return;
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    strncpy(outFormula, s.sensor[channel].formula, maxLen);
    outFormula[maxLen - 1] = '\0'; // null-terminate
  }
  else{
    strncpy(outFormula, "H2O", maxLen);
    outFormula[maxLen - 1] = '\0'; // null-terminate
  }
}

void Config::writeSensorFormula(uint8_t channel, const char* newFormula) {
 
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    memset(s.sensor[channel].formula, 0, sizeof(s.sensor[channel].formula));
    strncpy(s.sensor[channel].formula, 
            newFormula, sizeof(s.sensor[channel].formula));
    s.sensor[channel].formula[sizeof(s.sensor[channel].formula) - 1] = '\0';
    writeSettings(s);
  }
}

//how many decimal places to display
//uint8_t DataHandler::getDecimal(uint8_t channel){
//  return 2;
//}

uint8_t Config::readDecimal(uint8_t channel){
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    return s.sensor[channel].decimal;
  }
  else return 0;
}

void Config::writeSensorSettings
(uint8_t channel, SensorSettings& sensorSettings){
  
  if(channel < MAX_SENSORS_CONF){
    Settings s;
    readSettings(s);
    s.sensor[channel] = sensorSettings;
    writeSettings(s);
  }
}

Config::Config(){}

void Config::readSettings(Settings& out) const {
  const void* ptr = reinterpret_cast<const void*>(SEG_B_ADDR);
   memcpy(&out, ptr, sizeof(Settings));
}


void Config::writeSettings(Settings& s) { 
  
  s.check_sum = calculateChecksum(s);
  const uint16_t* src = reinterpret_cast<const uint16_t*>(&s);
  volatile uint16_t* dst = reinterpret_cast<volatile uint16_t*>(SEG_B_ADDR); 
  uint16_t words = sizeof(Settings) / sizeof(uint16_t);  
//  debug_s = s; //fd 

  __disable_interrupt();
  FCTL2 = FWKEY + FSSEL0 + FN1;
  FCTL3 = FWKEY;

  // Erase segment B (0x1000–0x107F)
  FCTL1 = FWKEY + ERASE;
  *(volatile uint16_t*)SEG_B_ADDR = 0;
  while (FCTL3 & BUSY);

  // Erase segment A (0x1080–0x10FF)
  FCTL1 = FWKEY + ERASE;
  *(volatile uint16_t*)SEG_A_ADDR = 0;
  while (FCTL3 & BUSY);

  // Now write both
  FCTL1 = FWKEY + WRT;
  for (uint16_t i = 0; i < words; ++i) {
      dst[i] = src[i];
      while (FCTL3 & BUSY);
  }

  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
  __enable_interrupt();
}

uint16_t Config::calculateChecksum(const Settings& settings) {
  // exclude check_sum
  const size_t numBytes = sizeof(Settings) - sizeof(uint16_t); 
  const uint8_t* data = reinterpret_cast<const uint8_t*>(&settings);

  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  for (size_t i = 0; i < numBytes; ++i) {
    sum1 = (sum1 + data[i]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }

  return (sum2 << 8) | sum1;
}

const char*  Config::readUnits(uint8_t channel){
  if(channel < MAX_SENSORS_CONF){
    if(!DataHandler::instance().isSensorInitialized(channel))return MV;
    Settings s;
    readSettings(s);
    switch(s.sensor[channel].units){
      case SensorSettings::MG_PER_M3:
        return MG_PER_M3;
      case SensorSettings::PERCENT:
        return PERCENT;
    }
  }
  return ERROR;
}

Config::CommonSettings::AlarmType Config::readAlarmType(){
  Settings s;
  readSettings(s);
  return s.common.alarmType;
}

void Config::writeAlarmType(CommonSettings::AlarmType type){
  Settings s;
  readSettings(s);
  s.common.alarmType = type;
  writeSettings(s);
}

  


