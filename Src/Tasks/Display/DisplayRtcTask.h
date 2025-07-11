// DisplayRtcTask.h
#pragma once
#include "DisplayTaskBase.h"
#include "RtcTask.h"


class DisplayRtcTask : public DisplayTaskBase {
public:
  DisplayRtcTask(IDisplay& display);
  virtual void execute();
};

