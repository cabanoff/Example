// Config.h

#pragma once

#include "main.h"



class Config {
public:
  Config() {}
  
  void init(){
    Settings s;
    readSettings(s);
    if(s.check_sum != calculateChecksum(s)){
      s.common = defaultCommonSettings;
      for (uint8_t i = 0; i < MAX_SENS_SEG; ++i) {
          s.sensor[i] = defaultSensorSettings;
      }
      writeSettings(s);
    }
  }

  uint16_t readModbusAddr() const {
      Settings s;
      readSettings(s);
      return s.common.modbusAddr;
  }

  void writeModbusAddr(uint16_t addr) {
      Settings s;
      readSettings(s);
      s.common.modbusAddr = addr;
      writeSettings(s);
  }
  
  uint16_t readBlockingTime() const {
      Settings s;
      readSettings(s);
      return s.common.blockingTime;
  }

  void writeBlockingTime(uint16_t time) {
      Settings s;
      readSettings(s);
      s.common.blockingTime = time;
      writeSettings(s);
  }
  
  float readAlarmTreshold_1() const {
      Settings s;
      readSettings(s);
      return s.common.alarmTreshold_1;
  }
  
  void writeAlarmTreshold_1(float treshold) {
      Settings s;
      readSettings(s);
      s.common.alarmTreshold_1 = treshold;
      writeSettings(s);
  }
  
  void readSensorName(char* outName, size_t maxLen, uint8_t channel) const {
    if(maxLen == 0) return;
    if(channel < MAX_SENS_SEG){
      Settings s;
      readSettings(s);
      strncpy(outName, s.sensor[channel].name, maxLen);
      outName[maxLen - 1] = '\0'; // null-terminate
    }
    else{
      strncpy(outName, "None", maxLen);
      outName[maxLen - 1] = '\0'; // null-terminate
    }
  }

  void writeSensorName(const char* newName, uint8_t channel) {
    if(channel < MAX_SENS_SEG){
      Settings s;
      readSettings(s);
      strncpy(s.sensor[channel].name, newName, sizeof(s.sensor[channel].name));
      writeSettings(s);
    }
  }
  

private:
  // Internal flash segments
  static const uint16_t SEG_A_ADDR = 0x1080;
  static const uint16_t SEG_B_ADDR = 0x1000;
  
  static const uint8_t  MAX_SENS_SEG = 3;

  // Structures stored in flash
  struct CommonSettings {
      uint16_t  modbusAddr;   // offset 0
      uint16_t  blockingTime;
      int16_t   pressureCompensation;        
      float     alarmTreshold_1;    
      float     alarmTreshold_2;
  };

  struct SensorSettings {
      char name[30];
      char formula[12];
      float PDK_work_zone;     // PDK for working zone
      float PDK_max_single;    // maximum single PDK
      float PDK_day_average;   // average daily PDK 
  };
  
  struct Settings{
    SensorSettings sensor[3];
    CommonSettings common;
    uint16_t check_sum;
  };
  
  static const CommonSettings defaultCommonSettings;
  static const SensorSettings defaultSensorSettings;


  // Flash read/write functions
  
  void readSettings(Settings& out) const {
    const void* ptr = reinterpret_cast<const void*>(SEG_B_ADDR);
     memcpy(&out, ptr, sizeof(Settings));
  }

  
  void writeSettings(Settings& s) { 
    
    s.check_sum = calculateChecksum(s);
    const uint16_t* src = reinterpret_cast<const uint16_t*>(&s);
    volatile uint16_t* dst = reinterpret_cast<volatile uint16_t*>(SEG_B_ADDR); 
    uint16_t words = sizeof(Settings) / sizeof(uint16_t);  
    

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
  
  uint16_t calculateChecksum(const Settings& settings) {
    const size_t numWords = 
      (sizeof(Settings) - sizeof(uint16_t)) / sizeof(uint16_t);
    
    const uint16_t* p = reinterpret_cast<const uint16_t*>(&settings);
    uint16_t sum = 0;

    // XOR all 16-bit words in the structure 
    //(excluding the last word which is check_sum)
    for (size_t i = 0; i < numWords; ++i) {
      sum ^= p[i];
    }

    return sum;
  }
  
};




const Config::CommonSettings Config::defaultCommonSettings = {
   1,   
   840,
   -10,        
   1.0,    
   2.0
};

const Config::SensorSettings Config::defaultSensorSettings = {
   "None",
   "H2O",
    10,     // PDK for working zone
    10,    // maximum single PDK
    10   // average daily PDK 
};
