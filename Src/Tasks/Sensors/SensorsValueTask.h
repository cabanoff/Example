 // SensorsValueTask.h
#pragma once
#include "SensorsTaskBase.h"

class SensorsValueTask : public SensorsTaskBase {
public:
  explicit SensorsValueTask(sensorsPollScript& script)
    :SensorsTaskBase(script) {};

protected:
  // Override core functionality
  //virtual void onConnect();
  virtual void handleResponse(bool answerReceived);
  virtual bool isChannelExist(uint8_t ch_index);
};
