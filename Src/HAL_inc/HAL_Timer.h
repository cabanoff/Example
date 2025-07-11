// Timer.h
#pragma once
#include "main.h"

class HAL_Timer {
public:
  virtual void init(int time_us) = 0;
  virtual void handleTimerInterrupt() = 0;
  
};