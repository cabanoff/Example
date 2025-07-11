// DisplayModbusAddrTask.h
#pragma once
#include "DisplayTaskBase.h"
#include "DataHandler.h"

class DisplayModbusAddrTask : public DisplayTaskBase {
public:
  DisplayModbusAddrTask(IDisplay& display);
  virtual void execute();
};

