// PollStateTask.cpp

#include "PollStateTask.h"


PollStateTask::PollStateTask(StateMachine& stateMachine,
                             AppEvent::Type e,
                             IDisplay& display,
                             ISender* sender) :
    StateTaskBase(stateMachine, e, display, sender),
    display_(display),
    displaySubstances(display),
    displayAcc(display),
    displayModbusAddr(display),
    displayRtc(display),
    displayAlarm(display)
    {
      Scheduler::instance().connectTask(this);
    }
    


void PollStateTask::readSensorsRoutine(Routine::Type state, uint32_t time)
{
  if (state == Routine::ENTER) {
    
    display_.clrscr();
    /*************connect RTC*************/
    displayRtc.setPeriod(1000UL);
    routineHelper.registerDisplayTask(&displayRtc); 
    
    displayAlarm.setPeriod(1000UL);
    routineHelper.registerDisplayTask(&displayAlarm);
    
    displayAcc.setPeriod(5000UL);
    routineHelper.registerDisplayTask(&displayAcc);
  
    displayModbusAddr.setPeriod(10000UL);  // to do: set as event listener
    routineHelper.registerDisplayTask(&displayModbusAddr);
    
    displaySubstances.setPeriod(2000UL);
    routineHelper.registerDisplayTask(&displaySubstances);
        
    poll.singleCycleExecution = false;
    poll.toRBLabSensParser = RBLabSensCommand::ASK_CONC;
    
    routineHelper.activateSensorsPoll(new SensorsValueTask(poll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.disconnectAllDisplayTasks();
    routineHelper.deactivateSensorsPoll();
  }
}



void PollStateTask::run(ccrContParam) {
  ccrBeginContext;
  uint16_t delayCount; // Ticks remaining in delay
  ccrEndContext(ctx);

  ccrBegin(ctx);
    
  
  /**************************************************** 
  *           read concentration 
  *****************************************************/
  
  readSensorsRoutine(Routine::ENTER);
  
  while(true){
    ccrDelay(ctx, 1);
  }
   
  ccrFinishV;
}
  




  