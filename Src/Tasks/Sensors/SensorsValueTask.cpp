 // SensorsValueTask.cpp

#include  "SensorsValueTask.h"

struct DebugValid_{
  uint8_t channel;
  bool valid;
};
DebugValid_ debugValid_[MAX_SENSORS];


void SensorsValueTask::handleResponse(bool answerReceived){ 
  // Update validity state based on sensor response
  debugValid_[ch_index].channel = ch_index;
  debugValid_[ch_index].valid = answerReceived;
  DataHandler::instance().setValid(ch_index,answerReceived);  
}


bool SensorsValueTask::isChannelExist(uint8_t ch_index) {
  // Only poll channels with active port assignments
  return DataHandler::instance().isSensorConnected(ch_index);
}





