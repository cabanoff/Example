// HAL_UART1.h
#pragma once
#include "HAL_UART.h"


/**
 * @class HAL_UART1
 * @brief UART1 implementation for communication with the PC.
 */
class HAL_UART1 : public HAL_UART {
  friend class Mediator;
  
public:
  virtual bool send(const char* data, size_t length);
  virtual void init();
  
  void out(bool on);
  
  inline void handleRXInterrupt(char in_char);
  inline void handleTXInterrupt(void);
  
  static HAL_UART1* instance;
  
  // Callback support
  typedef void (*RxCallback)(char byte, void* context);
  typedef void (*TxCallback)(void* context);

  void connectRxCallback(RxCallback cb, void* ctx);
  void disconnectRxCallback();

  void connectTxCallback(TxCallback cb, void* ctx);
  void disconnectTxCallback();
  
private:
  HAL_UART1();
  
  static const uint16_t BUFFER_SIZE = UARTS_BUFFER_SIZE;
  bool isTXReady();
  bool flOut;
  
//  struct Buffer {
//    volatile uint8_t buffer[BUFFER_SIZE];
//    volatile uint16_t index;
//    volatile uint16_t end;
//  };
  
  struct RingBuffer {
    volatile uint8_t buffer[BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;

    inline bool isEmpty() const volatile {
      uint16_t localHead = head;
      uint16_t localTail = tail;
      return localHead == localTail;
    }
    inline bool isFull() const volatile {
      uint16_t next = (head + 1) % BUFFER_SIZE;
      return next == tail;
    }
    inline void reset() volatile
    { head = tail = 0; }
  };
  
  //static volatile Buffer rx;
//  static volatile Buffer tx;
  static volatile RingBuffer tx;
  
  RxCallback rxCallback;
  void* rxContext;
  TxCallback txCallback;
  void* txContext;
  
  void startTransmission(); 
  
};
