// IMediator.h
/**
* interface
*/
#pragma once
#include "IMediatorObserver.h"


class IMediator {
public:
  virtual ~IMediator() {}
  virtual observer_error_code connectObserver(IMediatorObserver* observer, 
                                              MediatorEvent::Type event) = 0;
  virtual observer_error_code disconnectObserver(IMediatorObserver* observer) = 0;
};