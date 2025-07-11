// IObserver.h
/**
* is a minimal abstract base class, 
* used as a common interface
* for all observers
*/
#pragma once
#include "main.h"

// Error codes used across task-related interfaces
typedef enum {
    OBSERVER_OK = 0,
    OBSERVER_ALREADY_ACTIVE,
    OBSERVER_NOT_ACTIVE,
    OBSERVER_INVALID_PARAM,
    OBSERVER_HANDLER_FULL,
    OBSERVER_HANDLER_INVALID
} observer_error_code;


class IObserver {
public:
  virtual ~IObserver() {}
};