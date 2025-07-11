// Config.h

//saves the data to the internal flash memory

#pragma once

#include "main.h"
#include "Singleton.h"
#include "DataHandler.h"



class Config : public Singleton<Config> {
  friend class Singleton<Config>;
public:
  // Structures stored in flash
  
  struct CommonSettings {
      uint16_t  modbusAddr;   
      int16_t   blockingTime;
      int16_t   pressureCompensation;        
      float     alarmThreshold_1;    
      float     alarmThreshold_2;
      enum AlarmType{
        ONE_THRESHOLD,
        TWO_THRESHOLD,
      }alarmType;
      enum PDKType{
        PDK_MAX_ALLOWED, 
        PDK_WORK_ZONE,
      }pdkType;
  };
  
  struct SensorSettings {
      char name[30];
      char formula[12];
      float start_range;     
      float end_range;   
      float threshold_1;    
      float threshold_2;
      enum AlarmDirection{
        UP,
        DOWN,
      }alarm1Direction,
      alarm2Direction;
      enum UnitType{
        MG_PER_M3, 
        PERCENT,
      }units;
  };
  

  

  
  
  void init();
  
  void getVersion(uint8_t version[5]) const;

  uint16_t readModbusAddr() const ;
  void writeModbusAddr(uint16_t addr);
  
  int16_t readBlockingTime() const ;
  void writeBlockingTime(int16_t time);
  
  int16_t readPressureCompensation() const ;
  void writePressureCompensation(int16_t val);
  
  float readAlarmThreshold_1() const;  
  void writeAlarmThreshold_1(float threshold);
  
  float readAlarmThreshold_2() const;  
  void writeAlarmThreshold_2(float threshold);
  
  void readSensorName(char* outName, size_t maxLen, uint8_t channel) const;
  void writeSensorName(const char* newName, uint8_t channel);
  
  uint8_t readPDKType();
  void writePDKType(uint8_t type);
  
  CommonSettings::AlarmType readAlarmType();
  
  float readThreshold_1(uint8_t channel) const ;
  float readThreshold_2(uint8_t channel) const ;
  
  SensorSettings::AlarmDirection threshold1Direction(uint8_t channel);
  SensorSettings::AlarmDirection threshold2Direction(uint8_t channel);
  
  
  const char* readUnits(uint8_t channel);

private:
  
 
  // Internal flash segments
  static const uint16_t SEG_A_ADDR = 0x1080;
  static const uint16_t SEG_B_ADDR = 0x1000;
  
  static const uint8_t MAX_SENSORS_CONF = MAX_SENSORS;
  
  static const CommonSettings defaultCommonSettings;
  static const CommonSettings maxCommonSettings;
  static const CommonSettings minCommonSettings;
  static const SensorSettings defaultSensorSettings;
  
  static const char MG_PER_M3[8];
  static const char PERCENT[4];
  static const char MV[5];
  static const char ERROR[4];

  struct Settings{
    SensorSettings sensor[MAX_SENSORS_CONF];
    CommonSettings common;
    uint16_t init;
    uint16_t check_sum;
  };
  
  STATIC_ASSERT(sizeof(Config::Settings)%2 == 0,
                structure_must_be_2_bytes_padded);
  
  STATIC_ASSERT(sizeof(Config::Settings) < 0x100,
                structure_must_be_less_than_257_bytes);

  Config();
  
   // Flash read/write functions
  void readSettings(Settings& out) const;
  void writeSettings(Settings& s);
  
  uint16_t calculateChecksum(const Settings& settings);
};

