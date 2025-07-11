// ScheduledTaskBase.h
#pragma once
#include "IScheduledTask.h"
#include "coroutine.h"

class ScheduledTaskBase : public IScheduledTask {
public:
  ScheduledTaskBase() : period_ms_(1) {}

  virtual void setPeriod(uint16_t period_ms) {
    period_ms_ = period_ms;
  }

  virtual uint16_t getPeriod() const {
    return period_ms_;
  }

protected: 
  virtual void onConnect() {
    execute();
  }

private:
  uint16_t period_ms_;
};