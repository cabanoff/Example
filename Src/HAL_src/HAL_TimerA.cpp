// HAL_TimerA.cpp
#include "HAL_TimerA.h"


HAL_TimerA* HAL_TimerA::instance = NULL;

HAL_TimerA::HAL_TimerA(int time_us)
{
  instance = this;
  init(time_us);
}

void HAL_TimerA::init(int time_us){
  // Timer A0 initialization for 100µs interrupts
  STATIC_ASSERT(CLOCK_KHZ == 4000,frequency_must_be_4000kHz);
  TACCR0 = (time_us*4) - 1;  // 400 cycles (100µs @ 4MHz SMCLK)
                    
  TACCTL0_bit.CCIE = 1;
  TACTL_bit.TACLR = 1;
  
  TACTL |= TASSEL_1;
  TACTL |= MC_1;  /* Timer A mode control: 1 - Up to CCR0 */
}

void HAL_TimerA::set_uart_timeout_100us(UartChannel ch, 
                                        uint16_t timeout){
  const uint8_t ta_ie_saved = TACCTL0_bit.CCIE;
  TACCTL0_bit.CCIE = 0;          // critical section: pause Timer-A ISR
  uartTimeouts[ch].counter = timeout;
  TACCTL0_bit.CCIE = ta_ie_saved;
}

void HAL_TimerA::connectUARTTimeOutCallback(UartChannel ch, 
                                            UARTTimeOutCallback cb, 
                                            void* ctx){
  const uint8_t ta_ie_saved = TACCTL0_bit.CCIE;
  TACCTL0_bit.CCIE = 0;
  uartTimeouts[ch].callback = cb;
  uartTimeouts[ch].context = ctx;
  TACCTL0_bit.CCIE = ta_ie_saved;
}

void HAL_TimerA::disconnectUARTTimeOutCallback(UartChannel ch){
  const uint8_t ta_ie_saved = TACCTL0_bit.CCIE;
  TACCTL0_bit.CCIE = 0;
  uartTimeouts[ch].callback = 0;
  uartTimeouts[ch].context = 0;
  uartTimeouts[ch].counter = 0;
  TACCTL0_bit.CCIE = ta_ie_saved;
}


void HAL_TimerA::handleTimerInterrupt(){
  for (int i = 0; i < UART_CALL_BACKS_COUNT; ++i) {
    TimeoutState& t = uartTimeouts[i];
    if (t.counter) {
      if (--t.counter == 0){
        if(t.callback)t.callback(t.context);
      }
    }
  }
}


