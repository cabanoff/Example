 // SensorsZeroStartTask.cpp

#include  "SensorsZeroStartTask.h"

/***
* Send command :[8d 37 66 32 35 00] "start zero calibration" to sensors
* Expects response :[fb 66 65 63 33 00] "calibration is started"
***/




void SensorsZeroStartTask::handleResponse(bool answerReceived) {
  // Update zero calibration start status
  DataHandler::instance().setZeroStart(ch_index, answerReceived);
}

bool SensorsZeroStartTask::isChannelExist(uint8_t ch_index) {
  // Only process channels with active port assignments
  return DataHandler::instance().isSensorConnected(ch_index);
}


