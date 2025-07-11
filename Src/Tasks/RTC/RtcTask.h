//RtcTask.h

#pragma once

#include "ScheduledTaskBase.h"
#include "Singleton.h"

class RtcTask : public ScheduledTaskBase, public Singleton<RtcTask> {
  
  friend class Singleton<RtcTask>;// Allow Singleton to call private constructor

private: 
  
  RtcTask();
  
public:
  
  void init();
  
  void getDateTime(char* dateTime, uint8_t length);
  void setDateTime(const uint8_t* date_time);
  
  virtual void execute();
    
private:
  uint32_t timestamp;
  uint32_t set_time;
  static const uint32_t BUILD_TIMESTAMP;
  static const uint32_t UTC_TIMESTAMP; 
  
};