// ZeroStateTask.cpp

#include "ZeroStateTask.h"


ZeroStateTask::ZeroStateTask(StateMachine& stateMachine,
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
    
    

void ZeroStateTask::startZeroRoutine(Routine::Type state, uint32_t time)
{
  if(state == Routine::ENTER){
    display_.clrscr();
    display_.printStr(4,1,"Установка нуля ...");
    
    initPoll.singleCycleExecution = true;
    initPoll.duration_ms = 1000UL * time;
    initPoll.toRBLabSensParser = RBLabSensCommand::ZERO_SET;
  
    routineHelper.activateSensorsPoll(new SensorsZeroStartTask(initPoll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.deactivateSensorsPoll();  
  }
}


void ZeroStateTask::readZeroRoutine(Routine::Type state, uint32_t time)
{
  if (state == Routine::ENTER){
    
    initPoll.singleCycleExecution = false;
    initPoll.duration_ms = 1000UL * time;
    initPoll.toRBLabSensParser = RBLabSensCommand::ASK_ZERO_END;
  
    routineHelper.activateSensorsPoll(new SensorsZeroPollTask(initPoll));
    
  } else if (state == Routine::EXIT) {
    
    routineHelper.deactivateSensorsPoll();
  }
}

void ZeroStateTask::readSensorsRoutine(Routine::Type state, uint32_t time)
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


void ZeroStateTask::run(ccrContParam) {
  ccrBeginContext;
  uint8_t i;
  uint16_t delayCount; // Ticks remaining in delay
  ccrEndContext(ctx);

  ccrBegin(ctx);
    
  /**************************************************** 
  *              start zero calibration 
  *****************************************************/
    
  startZeroRoutine(Routine::ENTER,START_ZERO_TIME);
  
  DEBUG_PRINT("Connect sensorsZeroStartTask ");
  
  ccrDelay(ctx, START_ZERO_TIME);
  
  startZeroRoutine(Routine::EXIT);
  
  DEBUG_PRINT("Disconnect sensorsZeroStartTask ");
  
 /**************************************************** 
  *             read zero calibration 
  *****************************************************/
  
  readZeroRoutine(Routine::ENTER,ZERO_POLL_TIME);
  
  for(ctx->i = 0; ctx->i < (ZERO_POLL_TIME / 2); ++ctx->i){
    if (routineHelper.isSensorPollTaskComplete()) break;
    ccrDelay(ctx, 2);
  }
  
  readZeroRoutine(Routine::EXIT);
  
  /**************************************************** 
  *   read concentration with alarm blocked 
  *****************************************************/
  display_.clrscr();
  display_.printStr(9,0,"Блок.тревоги: ");  
   
  readSensorsRoutine(Routine::ENTER, ALARM_BLOCK_LEFT_TIME);
  
  ccrDelay(ctx, ALARM_BLOCK_LEFT_TIME);
  
  readSensorsRoutine(Routine::EXIT);

  DEBUG_PRINT("Disconnect SensorsZeroPollTask ");

  display_.clrscr();

  stateMachine.setEvent(outEvent);

  coroutineOnce = true;
  
  ccrFinishV;
}
  

