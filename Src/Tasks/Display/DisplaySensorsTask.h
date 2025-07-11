// DisplaySensorsTask.h
#pragma once
#include "DisplayTaskBase.h"
#include "DataHandler.h"
#include "Config.h"

class DisplaySensorsTask : public DisplayTaskBase {
public:
  DisplaySensorsTask(IDisplay& display);
  virtual void execute();

private: 
  const char* valueToDisplay(float value, uint8_t decimal,const char* units);
  uint8_t length(const char* str);
};