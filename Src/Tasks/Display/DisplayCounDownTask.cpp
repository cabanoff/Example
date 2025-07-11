// DisplayCounDownTask.cpp

#include "DisplayCounDownTask.h"

DisplayCountDownTask::DisplayCountDownTask(IDisplay& display)
  : DisplayTaskBase(display),
    countDown_(0),
    row_(0),
    col_(0) 
    {}

void DisplayCountDownTask::execute() {
  if (countDown_ > 0) {
    countDown_--;

    char buf[6];
    uint16_t min = countDown_ / 60;
    uint16_t sec = countDown_ % 60;

    buf[0] = '0' + (min / 10);
    buf[1] = '0' + (min % 10);
    buf[2] = ':';
    buf[3] = '0' + (sec / 10);
    buf[4] = '0' + (sec % 10);
    buf[5] = '\0';

    display_.printStr(row_, col_, buf);
  }
}

void DisplayCountDownTask::onConnect() {
  countDown_++;
  execute();
}

void DisplayCountDownTask::setCounter(uint16_t counter) {
  countDown_ = counter;
}

void DisplayCountDownTask::setPosition(uint8_t row, uint8_t col) {
  row_ = row;
  col_ = col;
}