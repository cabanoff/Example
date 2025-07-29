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
      uint8_t   pumpPower;
      float     alarmThreshold_1;    
      float     alarmThreshold_2;
      enum AlarmType{
        ONE_THRESHOLD,
        TWO_THRESHOLD,
        INDIVIDUAL_RELAY,
      }alarmType;
      enum PDKType{
        PDK_MAX_ALLOWED, 
        PDK_WORK_ZONE,
      }pdkType;
  };
  
  struct SensorSettings {
      char name[MAX_NAME_LENGTH];
      char formula[MAX_FORMULA_LENGTH];
      float start_range;     
      float end_range;   
      float threshold_1;    
      enum AlarmDirection{
        UP,
        DOWN,
      }alarm1Direction;
      float threshold_2;
      AlarmDirection alarm2Direction;
      enum UnitType{
        MG_PER_M3, 
        PERCENT,
      }units;
      uint8_t decimal;
  };

  struct Settings{
    SensorSettings sensor[MAX_SENSORS];
    CommonSettings common;
    uint16_t init;
    uint16_t check_sum;
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
  
  void readSensorName(uint8_t channel, char* outName, size_t maxLen) const;
  void writeSensorName( uint8_t channel, const char* newName);
  
  void readSensorFormula
    (uint8_t channel, char* outFormula, size_t maxLen) const;
  void writeSensorFormula( uint8_t channel, const char* newFormula);
  
  CommonSettings::PDKType readPDKType();
  void writePDKType(CommonSettings::PDKType type);
  
  CommonSettings::AlarmType readAlarmType();
  void writeAlarmType(CommonSettings::AlarmType type);
  
  float readThreshold_1(uint8_t channel) const ;
  float readThreshold_2(uint8_t channel) const ;
  
  float readStartRange(uint8_t channel) const ;
  void writeStartRange(uint8_t channel, float start_range);
  
  SensorSettings::AlarmDirection readThreshold1Direction(uint8_t channel);
  SensorSettings::AlarmDirection readThreshold2Direction(uint8_t channel);
  
  uint8_t readDecimal(uint8_t channel);
  
  void writeSensorSettings(uint8_t channel, SensorSettings& sensorSettings);
  void readSensorSettings
    (uint8_t channel, SensorSettings& sensorSettings) const;
  
  
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

