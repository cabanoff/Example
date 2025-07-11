// DisplayAccTask.h
#pragma once
#include "DisplayTaskBase.h"

class DisplayAccTask : public DisplayTaskBase {
public:
  DisplayAccTask(IDisplay& display);
  virtual void execute();
};

