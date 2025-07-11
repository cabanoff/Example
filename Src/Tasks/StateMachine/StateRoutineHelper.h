// StateRoutineHelper.h
#pragma once

#pragma once
#include "Scheduler.h"
#include "Mediator.h"
#include "SensorsTaskBase.h"
#include "ISender.h"
#include "IDisplay.h"
#include "debug_utils.h"

class StateRoutineHelper {
public:
  StateRoutineHelper(IDisplay& display, ISender* sender) :
    display(display), 
    sensorsPoll(0), 
    sender(sender),
    displayTaskCount(0)
    {}

  ~StateRoutineHelper() {
    deactivateSensorsPoll();
  }

  void activateSensorsPoll(SensorsTaskBase* task) {
    sensorsPoll = task;
    Mediator::instance().connectObserver(sensorsPoll, MediatorEvent::RX0);
    sensorsPoll->connectSender(sender);
    Scheduler::instance().connectTask(sensorsPoll);
  }

  void deactivateSensorsPoll() {
    if (sensorsPoll) {
      Mediator::instance().disconnectObserver(sensorsPoll);
      Scheduler::instance().disconnectTask(sensorsPoll);
      delete sensorsPoll;
      sensorsPoll = 0;
    }
  }

  
  bool isSensorPollTaskComplete() {
    return sensorsPoll && sensorsPoll->isComplete();
  }

  IDisplay& getDisplay() { return display; }
  
  void registerDisplayTask(ScheduledTaskBase* task) {
    if (displayTaskCount >= MAX_DISPLAY_TASKS) {
      DEBUG_PRINT("Too many display tasks registered.");
      return;
    }

    // Prevent duplicate registration
    for (uint8_t i = 0; i < displayTaskCount; ++i) {
      if (connectedDisplays[i] == task) return;
    }

    Scheduler::instance().connectTask(task);
    connectedDisplays[displayTaskCount++] = task;
  }

  void disconnectAllDisplayTasks() {
    for (uint8_t i = 0; i < displayTaskCount; ++i) {
      Scheduler::instance().disconnectTask(connectedDisplays[i]);
    }
    displayTaskCount = 0;
  }

private:
  IDisplay& display;
  SensorsTaskBase* sensorsPoll;
  ISender* sender;
  
  static const uint8_t MAX_DISPLAY_TASKS = 5;
  ScheduledTaskBase* connectedDisplays[MAX_DISPLAY_TASKS];
  uint8_t displayTaskCount;

  // Prevent copying
  StateRoutineHelper(const StateRoutineHelper&);
  StateRoutineHelper& operator=(const StateRoutineHelper&);
};
