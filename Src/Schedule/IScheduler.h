// IScheduler.h
/**
* Interface
*/

#pragma once
#include "IScheduledTask.h"

class IScheduler {
public:
  virtual ~IScheduler() {}
  virtual task_error_code connectTask(IScheduledTask* task) = 0;
  virtual task_error_code disconnectTask(IScheduledTask* task) = 0;
  virtual void run() = 0;
};