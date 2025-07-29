// AlarmTask.cpp
#include "AlarmTask.h"

/**
*  responsible for alarm handle  
*
*/

AlarmTask::AlarmTask(IDisplay& display)
  : DisplayTaskBase(display), siren_counter(0) {
    AlarmGPIO::instance().init(); 
    alarmType = Config::instance().readAlarmType();  //one or two thresholds
    for (int i = 0; i < MAX_SENSORS_W_ALARM; ++i) {
      Alarms& ch = sensor_alarms[i];
      ch.threshold_1 = Config::instance().readThreshold_1(i);
      ch.threshold_2 = Config::instance().readThreshold_2(i);
      
      ch.threshold1Dir = Config::instance().readThreshold1Direction(i);
      ch.threshold2Dir = Config::instance().readThreshold2Direction(i);
      
      bool isUp = (ch.threshold1Dir == Config::SensorSettings::UP);
      ch.alarm_1_release = ch.threshold_1 * (isUp ? 0.8 : 1.2);      
      isUp = (ch.threshold2Dir == Config::SensorSettings::UP);
      ch.alarm_2_release = ch.threshold_2 * (isUp ? 0.8 : 1.2);
      
      ch.alarm1 = false;
      ch.alarm2 = false;
    }
  }

void AlarmTask::execute() {
#define TWO_THRESHOLD Config::CommonSettings::TWO_THRESHOLD
#define ONE_THRESHOLD Config::CommonSettings::ONE_THRESHOLD
#define INDIVIDUAL_RELAY Config::CommonSettings::INDIVIDUAL_RELAY
  
  bool any_alarm1 = false;
  bool any_alarm2 = false;

  for (int i = 0; i < MAX_SENSORS_W_ALARM; ++i) {
    if (DataHandler::instance().isSensorInitialized(i) &&
        DataHandler::instance().isDataValid(i)) 
    {
      Alarms& ch = sensor_alarms[i];
      //set alarm flags
      float value = DataHandler::instance().getConcentration(i);
      if(ch.alarm1){
        ch.alarm1 = (ch.threshold1Dir == Config::SensorSettings::UP) ?
        value > ch.alarm_1_release : value < ch.alarm_1_release;
      }
      else{
        ch.alarm1 = (ch.threshold1Dir == Config::SensorSettings::UP) ?
        value > ch.threshold_1 : value < ch.threshold_1;
      }
      
      if(ch.alarm2){
        ch.alarm2 = (ch.threshold2Dir == Config::SensorSettings::UP) ?
        value > ch.alarm_2_release : value < ch.alarm_2_release;
      }
      else{
        ch.alarm2 = (ch.threshold2Dir == Config::SensorSettings::UP) ?
        value > ch.threshold_2 : value < ch.threshold_2;
      }


      any_alarm1 |= ch.alarm1;
      any_alarm2 |= ch.alarm2;
      //perform individual alarm actions
      display_.printStr(1 + i * 3, 0, ch.alarm1 || ch.alarm2 ? "!" : " "); 
      //perform dependent individual alarm actions
      char* alarm_phrase;

      if (!(ch.alarm1 || ch.alarm2)) {
          alarm_phrase = "                   ";   // No alarms
      } else if (ch.alarm1 && ch.alarm2) {
          alarm_phrase = (alarmType == TWO_THRESHOLD) 
            ? "Alarm 2 is exceeded" 
            : "Alarm 1 is exceeded";  
      } else {
          alarm_phrase = (ch.alarm1)
            ? "Alarm 1 is exceeded"   // Only alarm1
            : "Alarm 2 is exceeded";  // Only alarm2
      } 
      if (alarmType == INDIVIDUAL_RELAY) {
        AlarmGPIO::instance().setRelay(i,ch.alarm1);
      }
      display_.printStr(3 + i * 3, 1, alarm_phrase);
    }
  }
  //perform common alarm actions
  switch(alarmType){
    case ONE_THRESHOLD:
      AlarmGPIO::instance().setRelay(0,any_alarm1);
      AlarmGPIO::instance().setRelay(1,false);
    break;
    case TWO_THRESHOLD:
      AlarmGPIO::instance().setRelay(0,any_alarm1);
      AlarmGPIO::instance().setRelay(1,any_alarm2);
    break;
  }
  siren(any_alarm1 || any_alarm2);
 
}

void AlarmTask::siren(bool alarm){
  if(alarm) {
    if(siren_counter%2)AlarmGPIO::instance().setSiren(true);
    else AlarmGPIO::instance().setSiren(false);
    siren_counter++;
  } else {
    AlarmGPIO::instance().setSiren(false);
    siren_counter = 0;
  }
}

void AlarmTask::onDisconnect() {
  AlarmGPIO::instance().setSiren(false);
  siren_counter = 0;
  for (int i = 0; i < MAX_SENSORS_W_ALARM; ++i) {
    AlarmGPIO::instance().setRelay(i,false);
  }
}




