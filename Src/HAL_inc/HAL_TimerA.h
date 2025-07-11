// HAL_TimerA.h
#pragma once
#include "HAL_Timer.h"

class HAL_TimerA : public HAL_Timer {
  friend class Mediator;
public:
  enum UartChannel { 
    UART0 = 0, 
    UART1, 
    TRANS, 
    UART_CALL_BACKS_COUNT 
  };
  
  void init(int time_us);
  
  static HAL_TimerA* instance;
  
  void handleTimerInterrupt();
  
  void set_uart_timeout_100us(UartChannel ch, uint16_t timeout);
  
  // Callback support
  typedef void (*UARTTimeOutCallback)(void* context);
  
  void connectUARTTimeOutCallback
    (UartChannel ch, UARTTimeOutCallback cb, void* ctx);
  
  void disconnectUARTTimeOutCallback(UartChannel ch);
    
private:
  HAL_TimerA(int time_us);
  
  struct TimeoutState {
    volatile uint16_t counter;
    UARTTimeOutCallback callback;
    void* context;
  };
  TimeoutState uartTimeouts[UART_CALL_BACKS_COUNT];
};
