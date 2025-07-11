// InitStateTask.h
#pragma once

#include "StateTaskBase.h"
#include "DisplaySensorsTask.h"
#include "DisplayAccTask.h"
#include "DisplayCounDownTask.h"
#include "DisplayModbusAddrTask.h"
#include "SensorsNameTask.h"
#include "SensorsValueTask.h"


class InitStateTask : public StateTaskBase{
public:
  explicit InitStateTask(StateMachine& stateMachine,
                AppEvent::Type e,
                IDisplay& display,
                ISender* sender);      

private:
  virtual void run(ccrContParam);
  virtual void onConnect();
  
  void setSensorsRoutine(Routine::Type state, uint32_t time = 0);
  void readSensorsRoutine(Routine::Type state, uint32_t time = 0);

  DisplaySensorsTask displaySubstances; 
  DisplayAccTask displayAcc;            //shows the battery status
  DisplayCountDownTask countDown;  
  DisplayModbusAddrTask displayModbusAddr;
  IDisplay& display_;
  
  sensorsPollScript initPoll;
  
};
