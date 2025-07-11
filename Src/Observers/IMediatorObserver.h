 // IMediatorObserver.h


#pragma once
#include "IObserver.h"
#include "ISender.h"

namespace MediatorEvent{
  enum Type{
    RX0 = 0,
    RX1,
    TX0,
    TX1,
    COUNT,  // helps with array sizing
    NONE
  };
}

class IMediatorObserver : public IObserver {
public:
  virtual ~IMediatorObserver() {}
  
  // Called by the Mediator when new data on UART arrives
  virtual void onMediatorEvent(const volatile uint8_t* data, 
                               uint16_t size, MediatorEvent::Type event) = 0;
  virtual void connectSender(ISender* sender) = 0;

protected:
  virtual bool processMessage(const volatile uint8_t* data, uint16_t size) = 0;
  ISender* sender;
};