// Scheduler.h
#pragma once
#include "IScheduler.h"
#include "HAL_Timer.h"
#include "Singleton.h"



class Scheduler : public IScheduler, public HAL_Timer, 
public Singleton<Scheduler> 
{
  friend class Singleton<Scheduler>;
public:
    // Init TimerB
  void init(int time_us);
  
  /**
 * Connects a new task to the scheduler.
 * Adds the task to the task list if there's space 
 * and it's not already present.
 *
 * @param task Pointer to the task to connect.
 * @return task_error_code indicating the result of the operation.
 */
  task_error_code connectTask(IScheduledTask* task );
  /**
 * Disconnects a task from the scheduler.
 * Removes the task from the task list and clears its associated state.
 *
 * @param task Pointer to the task to disconnect.
 * @return task_error_code indicating the result of the operation.
 */
  task_error_code disconnectTask(IScheduledTask* task);
  
  
  void run();
  
  void handleTimerInterrupt();
  
private:
  
  Scheduler();
  
  static const uint16_t MAX_TASKS = 10;
   
  IScheduledTask* tasks[MAX_TASKS];
  uint16_t period_holder[MAX_TASKS];
  volatile uint16_t period_counter[MAX_TASKS];
  volatile bool flags[MAX_TASKS];
  uint8_t numTasks;
   

};




