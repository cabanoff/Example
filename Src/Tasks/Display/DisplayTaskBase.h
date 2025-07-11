// DisplayTaskBase.h
#pragma once
#include "ScheduledTaskBase.h"
#include "IDisplay.h"

class DisplayTaskBase : public ScheduledTaskBase {
public:
  DisplayTaskBase(IDisplay& display) : display_(display) {}

protected:
  IDisplay& display_;
};