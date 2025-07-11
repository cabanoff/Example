// DisplayRtcTask.cpp
#include "DisplayRtcTask.h"

DisplayRtcTask::DisplayRtcTask(IDisplay& display)
  : DisplayTaskBase(display) {}

void DisplayRtcTask::execute() {
  char buf[18];
  RtcTask::instance().getDateTime(buf,sizeof(buf));
  display_.printStr(9,2,buf);
}