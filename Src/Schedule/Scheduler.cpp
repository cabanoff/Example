// Scheduler.cpp
#include "Scheduler.h"

uint8_t d_tasks_counter = 0;

inline void __disableTimerB_interrupt() {
  TBCCTL0_bit.CCIE = 0;  // Disable CCR0 interrupt
}

inline void __enableTimerB_interrupt() {
  TBCCTL0_bit.CCIE = 1;   // Enable CCR0 interrupt
}


Scheduler::Scheduler(): numTasks(0) {
  for (uint16_t i = 0; i < MAX_TASKS; i++) {
    tasks[i] = NULL;
    period_holder[i] = 0;
    period_counter[i] = 0;
    flags[i] = false;
  }
  init(1000);
}

  
    // Init TimerB
void Scheduler::init(int time_us){
  // Timer B0 initialization for 1000µs interrupts
  STATIC_ASSERT(CLOCK_KHZ == 4000,frequency_must_be_4000kHz);
  TBCCR0 = (time_us*4) - 1;  
                    
  __enableTimerB_interrupt(); /* Capture/compare interrupt enable  */
  TBCTL_bit.TBCLR = 1;        /* Timer B counter clear  */
  
  TBCTL |= TBSSEL_1;          /* Clock Source: ACLK  */
  TBCTL |= MC_1;	      /* Timer mode control: 1 - Up to CCR0 */
}
  
task_error_code Scheduler::connectTask(IScheduledTask* task ){
  if (!task || numTasks >= MAX_TASKS) return TASK_INVALID_PARAM;
  
  for(uint16_t i=0; i < MAX_TASKS; i++){
    if(tasks[i] == task) return TASK_ALREADY_ACTIVE;
  }
  
  __disableTimerB_interrupt();
  // Find free slot
  for(uint16_t i=0; i < MAX_TASKS; i++) {
    if(!tasks[i]) {
      tasks[i] = task;
      period_holder[i] = task->getPeriod();
      period_counter[i] = period_holder[i];
      __enableTimerB_interrupt();
      task->onConnect();
      numTasks++;
      if(d_tasks_counter < numTasks)d_tasks_counter = numTasks;
      return TASK_OK;
    }
  }
  __enableTimerB_interrupt();
  return HANDLER_FULL;
}

task_error_code Scheduler::disconnectTask(IScheduledTask* task) {
  if (!task) return TASK_INVALID_PARAM;
  task_error_code result = TASK_NOT_ACTIVE;
  __disableTimerB_interrupt();
  
  for(uint16_t i=0; i < MAX_TASKS; i++){
    if(tasks[i] == task) {
      tasks[i] = NULL;
      period_holder[i] = 0;
      period_counter[i] = 0;
      numTasks--;
      result = TASK_OK;
      break;
    }
  }
  __enableTimerB_interrupt();
  return result;
}
  
  
void Scheduler::run() {
  for(uint16_t i=0; i < MAX_TASKS; i++) {
    if(tasks[i] && flags[i]) {
      flags[i] = false;
      tasks[i]->execute();
    }
  }
}


void Scheduler::handleTimerInterrupt() {
  for(uint16_t i = 0; i < MAX_TASKS; i++) {
    if (tasks[i] && period_counter[i] > 0) {
      if (--period_counter[i] == 0) {
        flags[i] = true;
        period_counter[i] = period_holder[i];
      }
    }
  }
}
  


  




