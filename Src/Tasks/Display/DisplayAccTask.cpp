// DisplayAccTask.cpp
#include "DisplayAccTask.h"

DisplayAccTask::DisplayAccTask(IDisplay& display)
  : DisplayTaskBase(display) {}

void DisplayAccTask::execute() {
  display_.printAcc(4);
}