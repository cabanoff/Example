// ITask.h
/**
* is a minimal abstract base class, 
* used as a common interface
* for all task types — scheduled or reactive.
* Allows to treat all tasks generically
*/
#pragma once
#include "main.h"

// Error codes used across task-related interfaces
typedef enum {
    TASK_OK = 0,
    TASK_ALREADY_ACTIVE,
    TASK_NOT_ACTIVE,
    TASK_INVALID_PARAM,
    HANDLER_FULL,
    HANDLER_INVALID
} task_error_code;


class ITask {
public:
  virtual ~ITask() {}
};