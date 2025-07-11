// SensorsNameTask.h
#pragma once
#include "SensorsTaskBase.h"

class SensorsNameTask : public SensorsTaskBase {
public:
  explicit SensorsNameTask(sensorsPollScript& script) 
    : SensorsTaskBase(script) 
    {
      DataHandler::instance().init();
    }
  virtual ~SensorsNameTask();

protected:
  virtual void handleResponse(bool answerReceived);
  virtual bool isChannelExist(uint8_t ch_index);
  //virtual void handleDurationReached();
};
