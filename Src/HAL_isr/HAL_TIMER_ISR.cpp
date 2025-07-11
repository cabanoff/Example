// HAL_TIMER_ISR.cpp

#include "HAL_TimerA.h"
#include "Scheduler.h"


// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0_ISR(void) {
  HAL_TimerA::instance->handleTimerInterrupt();
  __low_power_mode_off_on_exit(); // Wake up after ISR
}

// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B0_ISR(void) {
    //(executes every 1ms)
  Scheduler::instance().handleTimerInterrupt();
  __low_power_mode_off_on_exit(); // Wake up after ISR
}