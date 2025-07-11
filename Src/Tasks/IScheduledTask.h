// IScheduledTask.h
/**
*
* lets the Scheduler handle only tasks that need periodic execution
* 
*/

#pragma once
#include "ITask.h"

class IScheduledTask : public ITask {
public:
  virtual ~IScheduledTask() {}

  virtual void execute() = 0;
  virtual void onConnect() = 0;

  virtual void setPeriod(uint16_t period_ms) = 0;
  virtual uint16_t getPeriod() const = 0;
};

