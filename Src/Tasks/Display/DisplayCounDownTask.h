// DisplayCounDownTask.h
#pragma once
#include "DisplayTaskBase.h"

class DisplayCountDownTask : public DisplayTaskBase {
public:
  DisplayCountDownTask(IDisplay& display);
  virtual void execute();
  virtual void onConnect(); 

  void setCounter(uint16_t counter);
  void setPosition(uint8_t row, uint8_t col);

private:
  uint16_t countDown_;
  uint8_t row_;
  uint8_t col_;
};