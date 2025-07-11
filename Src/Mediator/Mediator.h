//Mediator.h

#pragma once
#include "main.h"
#include "HAL_UART1.h"
#include "HAL_UART0.h"
#include "HAL_TimerA.h"
#include "IMediator.h"
#include "IMediatorObserver.h"
#include "Singleton.h"



class Mediator : public IMediator, public Singleton<Mediator> {
  friend class Singleton<Mediator>;
public:
  
  void onUart1ByteReceived(char byte);
  void onUart0ByteReceived(char byte);
  bool sendToUART1(const char* data, size_t length);
  bool sendToUART0(const char* data, size_t length);
  void onUart1Timeout();
  void onUart0Timeout();
  void onTransTimeout();
  void onUart0TxEnd();
  void onUart1TxEnd();
  void run();
  void setTransMode();
  void setNormalMode();
  
  virtual observer_error_code connectObserver(IMediatorObserver* observer, 
                                              MediatorEvent::Type event);
  virtual observer_error_code disconnectObserver(IMediatorObserver* observer);

private:
  Mediator(); // Private constructor

  //timeout to end uart message
  static const uint16_t TIMEOUT_2MS = 21; 
  static const uint16_t TIMEOUT_1MS = 11;
  
  static const uint16_t BUFFER_SIZE = UARTS_BUFFER_SIZE;
  //tick of timerA in usec
  static const uint16_t TIMER_A_TIC100US = 100;
  //static const uint16_t MAX_RX_OBSERVERS = 2;
   
  static void staticRxHandler1(char byte, void* ctx);
  static void staticTimeoutHandler1(void* ctx);
  static void staticTxHandler1(void* ctx);
  
  static void staticRxHandler0(char byte, void* ctx);
  static void staticTimeoutHandler0(void* ctx);
  static void staticTxHandler0(void* ctx);
  
  static void staticTimeoutHandlerTrans(void* ctx);
  
  struct Buffer {
    volatile uint8_t buffer[BUFFER_SIZE];
    volatile uint16_t index;
  };
  
  static volatile Buffer rx1,in_hold1;
  static volatile Buffer rx0, in_hold0;
  
  volatile bool eventReady[MediatorEvent::COUNT];

  bool UARTsTransMode;
  
  static const uint8_t MAX_OBSERVERS_PER_EVENT = 2;
  IMediatorObserver* eventObservers[MediatorEvent::COUNT][MAX_OBSERVERS_PER_EVENT];
  
  HAL_UART1     uart1;
  HAL_UART0     uart0;
  HAL_TimerA    timerA;

};