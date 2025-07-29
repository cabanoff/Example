// TransStateTask.h

#include "StateTaskBase.h"
#include "DisplayAccTask.h"
#include "DisplayModbusAddrTask.h"
#include "DisplaySensorsTask.h"
#include "RBLabParser.h"
#include "SensorsValueTask.h"


class TransStateTask : public StateTaskBase{
public:
  explicit TransStateTask(StateMachine& stateMachine,
                          AppEvent::Type e,
                          IDisplay& display,
                          ISender* sender);

private:
  
  virtual void run(ccrContParam);
  
  void readSensorsRoutine(Routine::Type state, uint32_t time = 0);
      
  IDisplay& display_; 
  DisplaySensorsTask displaySubstances;
  DisplayAccTask displayAcc;            //shows battery status 
  DisplayModbusAddrTask displayModbusAddr;
  RBLabCommand::Type command;
  
  sensorsPollScript poll;
  
};
