// AlarmTask.cpp
#include "AlarmTask.h"

/**
*  responsible for alarm handle  
*
*/

AlarmTask::AlarmTask(IDisplay& display)
  : DisplayTaskBase(display) {
    alarmType = Config::instance().readAlarmType();  //one or two thresholds
    for (int i = 0; i < MAX_SENSORS_W_ALARM; ++i) {
      sensor_alarms[i].threshold_1 = Config::instance().readThreshold_1(i);
      sensor_alarms[i].threshold_2 = Config::instance().readThreshold_2(i);
      sensor_alarms[i].threshold1Dir = 
        Config::instance().threshold1Direction(i);
      sensor_alarms[i].threshold2Dir = 
        Config::instance().threshold1Direction(i);
      if(sensor_alarms[i].threshold1Dir == Config::SensorSettings::UP){
        sensor_alarms[i].alarm_1_release = sensor_alarms[i].threshold_1 * 0.8;
      }else{
        sensor_alarms[i].alarm_1_release = sensor_alarms[i].threshold_1 * 1.2;
      }
      if(sensor_alarms[i].threshold2Dir == Config::SensorSettings::UP){
        sensor_alarms[i].alarm_2_release = sensor_alarms[i].threshold_2 * 0.8;
      }else{
        sensor_alarms[i].alarm_2_release = sensor_alarms[i].threshold_2 * 1.2;
      }
    }
  }

void AlarmTask::execute() {
  for (int i = 0; i < MAX_SENSORS_W_ALARM; ++i) {
    if (DataHandler::instance().isSensorInitialized(i)) {
      if(DataHandler::instance().getValid(i)){
        float value = DataHandler::instance().getConcentration(i);
        if(value > 0.0)display_.printStr(3 + i * 3, 0, "!");
        else display_.printStr(3 + i * 3, 0, " ");
      }
    }
  }
}


