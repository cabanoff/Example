// RBLabStateTask.cpp
// русские буквы

#include "RBLabStateTask.h"


RBLabStateTask::RBLabStateTask(StateMachine& stateMachine,
                               AppEvent::Type e,
                               IDisplay& display,
                               ISender* sender) :
    StateTaskBase(stateMachine, e, display, sender),
    command(RBLabCommand::NONE),
    display_(display),
    displaySubstances(display),
    displayAcc(display),
    displayModbusAddr(display)
    {
      this->setPeriod(10);
      Scheduler::instance().connectTask(this);
    }
    


void RBLabStateTask::readSensorsRoutine(Routine::Type state, uint32_t time)
{
  if (state == Routine::ENTER) {
    
    displayAcc.setPeriod(5000UL);
    routineHelper.registerDisplayTask(&displayAcc);
    
    displayModbusAddr.setPeriod(10000UL);  // to do: set as event listener
    routineHelper.registerDisplayTask(&displayModbusAddr);
    
    displaySubstances.setPeriod(2000UL); 
    routineHelper.registerDisplayTask(&displaySubstances);
        
//    poll.singleCycleExecution = false;
//    poll.toRBLabSensParser = RBLabSensCommand::ASK_CONC;
//    
//    routineHelper.activateSensorsPoll(new SensorsValueTask(poll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.disconnectAllDisplayTasks();
    routineHelper.deactivateSensorsPoll();
  }
}

//coroutine don't use switch() here
void RBLabStateTask::run(ccrContParam) {
  ccrBeginContext;
  uint16_t delayCount; // Ticks remaining in delay
  ccrEndContext(ctx);

  ccrBegin(ctx);
    
  
  /**************************************************** 
  *           wait command from RBLabParser
  *****************************************************/
 
  
  display_.clrscr();
  
  readSensorsRoutine(Routine::ENTER);

  display_.printStr(9,0,"–ежим упр-€ с –ЅЋаб");
  
  while(true){
    command = RBLabParser::instance().readCommandToMCU(); 
    ccrDelay(ctx, 1);
  }
  
  ccrFinishV;
}
  