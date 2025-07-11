 // SensorsZeroStartTask.h
#pragma once
#include "SensorsTaskBase.h"

class SensorsZeroStartTask : public SensorsTaskBase {
public:
  explicit SensorsZeroStartTask(sensorsPollScript& script)
    :SensorsTaskBase(script){};

protected:
  // Core functionality overrides
  virtual void handleResponse(bool answerReceived);
  virtual bool isChannelExist(uint8_t ch_index);
};