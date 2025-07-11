// SensorsZeroPollTask.cpp

#include  "SensorsZeroPollTask.h"
#include  "debug_utils.h"

/***
* Send command :[8d 37 66 32 35 00] "start zero calibration" to sensors
* Expects response :[fb 66 65 63 33 00] "calibration is started"
***/

void SensorsZeroPollTask::onConnect() {
  for(uint8_t i = 0; i < MAX_CHANNELS; ++i) {
    DataHandler::instance().setZeroFinish(i, false);
  }
}


void SensorsZeroPollTask::handleResponse(bool answerReceived) {
  DataHandler::instance().setZeroFinish(ch_index, answerReceived);
}


bool SensorsZeroPollTask::isChannelExist(uint8_t ch_index) {
  return DataHandler::instance().isSensorConnected(ch_index) &&
         DataHandler::instance().getZeroStart(ch_index) &&
         !DataHandler::instance().getZeroFinish(ch_index);
}


void SensorsZeroPollTask::onEndCycle() {
  // First handle base class logic
  SensorsTaskBase::onEndCycle();

  // Custom early completion check
  uint8_t completedChannels = 0;
  for(uint8_t i = 0; i < MAX_CHANNELS; ++i) {
    if(!isChannelExist(i)) completedChannels++;
  }

  if(completedChannels == MAX_CHANNELS){
    handleDurationReached();
  }
}



