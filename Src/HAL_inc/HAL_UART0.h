// HAL_UART0.h
#pragma once
#include "HAL_UART.h"

class HAL_UART0 : public HAL_UART {
  friend class Mediator;
  
public:
  virtual bool send(const char* data, size_t length);
  virtual void init();
    
  inline void handleRXInterrupt(char in_char);
  inline void handleTXInterrupt(void);
  
  static HAL_UART0* instance;  // Singleton instance
  
  // Callback support
  typedef void (*RxCallback)(char byte, void* context);
  typedef void (*TxCallback)(void* context);

  void connectRxCallback(RxCallback cb, void* ctx);
  void disconnectRxCallback();

  void connectTxCallback(TxCallback cb, void* ctx);
  void disconnectTxCallback();
  
private:
  HAL_UART0();
  
  static const uint16_t BUFFER_SIZE = UARTS_BUFFER_SIZE;
  bool isTXReady();

  
  struct Buffer {
    volatile uint8_t buffer[BUFFER_SIZE];
    volatile uint16_t index;
    volatile uint16_t end;
  };
  
  static volatile Buffer tx;
  
  RxCallback rxCallback;
  void* rxContext;
  TxCallback txCallback;
  void* txContext;
  
};