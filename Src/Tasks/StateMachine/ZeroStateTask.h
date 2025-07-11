// ZeroStateTask.h
#pragma once
#include "StateTaskBase.h"
#include "DisplaySensorsTask.h"
#include "DisplayAccTask.h"
#include "DisplayCounDownTask.h"
#include "DisplayModbusAddrTask.h"
#include "SensorsZeroStartTask.h"
#include "SensorsZeroPollTask.h"
#include "SensorsValueTask.h"

class ZeroStateTask : public StateTaskBase{
public:
  explicit ZeroStateTask(StateMachine& stateMachine,
                         AppEvent::Type e,
                         IDisplay& display,
                         ISender* sender);
    

private:
  virtual void run(ccrContParam);
   
  void startZeroRoutine(Routine::Type state, uint32_t time = 0);
  void readZeroRoutine(Routine::Type state, uint32_t time = 0);
  void readSensorsRoutine(Routine::Type state, uint32_t time = 0);
      
  IDisplay& display_;   
  sensorsPollScript initPoll;
  DisplaySensorsTask displaySubstances; 
  DisplayAccTask displayAcc;            //shows the battery status
  DisplayCountDownTask countDown;  
  DisplayModbusAddrTask displayModbusAddr;
  
};
