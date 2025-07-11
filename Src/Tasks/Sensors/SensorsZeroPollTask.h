// SensorsZeroPollTask.h
#pragma once
#include "SensorsTaskBase.h"

class SensorsZeroPollTask : public SensorsTaskBase {
public:
  explicit SensorsZeroPollTask(sensorsPollScript& script)
    :SensorsTaskBase(script) {};

protected:
  // Override crucial lifecycle methods
  virtual void onConnect();
  virtual void handleResponse(bool answerReceived);
  virtual bool isChannelExist(uint8_t ch_index);
  virtual void onEndCycle();
};