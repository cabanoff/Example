// InitStateTask.cpp

#include "InitStateTask.h"


InitStateTask::InitStateTask(StateMachine& stateMachine,
                             AppEvent::Type e,
                             IDisplay& display,
                             ISender* sender) :
    StateTaskBase(stateMachine, e, display, sender),
    displaySubstances(display),
    displayAcc(display),
    countDown(display),
    displayModbusAddr(display),
    display_(display)
    {
      Scheduler::instance().connectTask(this);
    }
    
    
void InitStateTask::onConnect(){
  //hardcoded for type approval cert.
  display_.clrscr();
  display_.printStr(1,3,"Газоанализатор");
  display_.printStr(3,6,"ГАНК-4М");
  display_.printStr(5,5,"Версия 8.21"); 
}


void InitStateTask::setSensorsRoutine(Routine::Type state, uint32_t time)
{

  if (state == Routine::ENTER) {
    
    display_.clrscr();
    display_.printStr(5,2,"Установка режима");
    display_.printStr(6,6,"сенсоров...");
    
    displayAcc.setPeriod(5000UL);
    routineHelper.registerDisplayTask(&displayAcc);
    
    displayModbusAddr.setPeriod(10000UL);
    routineHelper.registerDisplayTask(&displayModbusAddr);
    
    countDown.setPeriod(1000);
    countDown.setCounter(time);
    countDown.setPosition(8,7);
    routineHelper.registerDisplayTask(&countDown);
    
    initPoll.singleCycleExecution = true;
    initPoll.duration_ms = 1000UL * time;
    initPoll.toRBLabSensParser = RBLabSensCommand::ASK_SUBST_NAME;
    routineHelper.activateSensorsPoll(new SensorsNameTask(initPoll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.disconnectAllDisplayTasks();
    routineHelper.deactivateSensorsPoll();
  }
  
}

void InitStateTask::readSensorsRoutine(Routine::Type state, uint32_t time)
{
  if (state == Routine::ENTER) {
    
    displayAcc.setPeriod(5000UL);
    routineHelper.registerDisplayTask(&displayAcc);
  
    displayModbusAddr.setPeriod(10000UL);  // to do: set as event listener
    routineHelper.registerDisplayTask(&displayModbusAddr);
    
    displaySubstances.setPeriod(2000UL);
    routineHelper.registerDisplayTask(&displaySubstances);
    
    countDown.setPeriod(1000UL);
    countDown.setCounter(time);
    countDown.setPosition(9,14);
    routineHelper.registerDisplayTask(&countDown);
        
    initPoll.singleCycleExecution = false;
    initPoll.toRBLabSensParser = RBLabSensCommand::ASK_CONC;
    
    routineHelper.activateSensorsPoll(new SensorsValueTask(initPoll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.disconnectAllDisplayTasks();
    routineHelper.deactivateSensorsPoll();
  }
}


void InitStateTask::run(ccrContParam) {
  ccrBeginContext;
  uint16_t delayCount; // Ticks remaining in delay
  ccrEndContext(ctx);

  ccrBegin(ctx);
    
  ccrDelay(ctx, 2); //delay in cycles, one cycle = 1sec 
  
  /**************************************************** 
  *                read sensors names
  *****************************************************/  
  
  setSensorsRoutine(Routine::ENTER,INIT_TIME);
  
  DEBUG_PRINT("Connect SensorsNameTask ");
  
  ccrDelay(ctx, INIT_TIME); //delay in cycles, one cycle = 1sec 
  
  setSensorsRoutine(Routine::EXIT);
  
  DEBUG_PRINT("Disonnect SensorsNameTask ");
  
  /**************************************************** 
  *   read concentration with alarm blocked 
  *****************************************************/
  display_.clrscr();
  display_.printStr(9,0,"Блок.тревоги: ");  
   
  readSensorsRoutine(Routine::ENTER, ALARM_BLOCK_TIME );
  
  ccrDelay(ctx, ALARM_BLOCK_TIME - ALARM_BLOCK_LEFT_TIME);
  
  readSensorsRoutine(Routine::EXIT);
   
  
  DEBUG_PRINT("Disconnect SensorsValueTask ");

  display_.clrscr();
  display_.printStr(5,1,"Exit from Init mode");
  ccrDelay(ctx, 2);

  stateMachine.setEvent(outEvent);

  coroutineOnce = true;
  
  ccrFinishV;
}
  

