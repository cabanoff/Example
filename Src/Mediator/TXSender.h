// TXSender.h

#pragma once

#include "main.h"
#include "ISender.h"
#include "Mediator.h"

// UART1 sender
class TX1Sender : public ISender {
public:
  virtual bool send(const char* data, size_t length) {
    return Mediator::instance().sendToUART1(data, length);
  }
};

// UART0 sender
class TX0Sender : public ISender {
public:
  virtual bool send(const char* data, size_t length){
    return Mediator::instance().sendToUART0(data, length);
  }
};