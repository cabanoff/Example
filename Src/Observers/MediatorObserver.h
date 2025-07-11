// MediatorObserver.h

#pragma once

#include "IMediatorObserver.h"
#include "IParser.h"



class MediatorObserver : public IMediatorObserver {
public:
  
  MediatorObserver(IParser& parser);
  
  virtual ~MediatorObserver() {}

  virtual void onMediatorEvent(const volatile uint8_t* data, uint16_t size, 
                               MediatorEvent::Type event);
  
  virtual void connectSender(ISender* sender);
  

private:
  virtual bool processMessage(const volatile uint8_t* data, uint16_t size);
  IParser& parser;
  static const uint16_t OBSERVER_BUFFER = UARTS_BUFFER_SIZE;

};