// PollStateTask.h

#include "StateTaskBase.h"
#include "DisplaySensorsTask.h"
#include "DisplayAccTask.h"
#include "SensorsValueTask.h"
#include "DisplayModbusAddrTask.h"
#include "DisplayRtcTask.h"
#include "AlarmTask.h"


class PollStateTask : public StateTaskBase{
public:
  explicit PollStateTask(StateMachine& stateMachine,
                         AppEvent::Type e,
                         IDisplay& display,
                         ISender* sender);

private:
  
  virtual void run(ccrContParam);

  
  void readSensorsRoutine(Routine::Type state, uint32_t time = 0);

      
  IDisplay& display_; 
  DisplaySensorsTask displaySubstances; //shows a name and value 
                                        //of the substance
  DisplayAccTask displayAcc;            //shows battery status 
  DisplayModbusAddrTask displayModbusAddr;
  DisplayRtcTask displayRtc;
  AlarmTask displayAlarm;
 
  sensorsPollScript poll;
  
};
