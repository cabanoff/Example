// AlarmTask.h
#pragma once
#include "DisplayTaskBase.h"
#include "DataHandler.h"
#include "Config.h"
#include "AlarmGPIO.h"

class AlarmTask : public DisplayTaskBase {
public:
  AlarmTask(IDisplay& display);
  virtual void execute();

private: 
  
  static const uint8_t MAX_SENSORS_W_ALARM = MAX_SENSORS;
  
  Config::CommonSettings::AlarmType alarmType;
  struct Alarms{
    float threshold_1;
    float threshold_2;
    Config::SensorSettings::AlarmDirection threshold1Dir;
    Config::SensorSettings::AlarmDirection threshold2Dir;
    float alarm_1_release;
    float alarm_2_release;
    bool alarm1;
    bool alarm2;
  };
  Alarms sensor_alarms[MAX_SENSORS_W_ALARM];
  uint16_t siren_counter;
  
  void onDisconnect();
  void siren(bool alarm);
};