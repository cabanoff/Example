// SensorsNameTask.cpp
#include "SensorsNameTask.h"

struct DebugValid{
  uint8_t channel;
  bool valid;
};
DebugValid debugValid[MAX_SENSORS];


void SensorsNameTask::handleResponse(bool answerReceived) {
  if (!answerReceived) {
    DataHandler::instance().setPort(ch_index, Ports::NOT_CONNECTED);
  }
}

bool SensorsNameTask::isChannelExist(uint8_t ch_index) {
  return DataHandler::instance().isSensorConnected(ch_index);
}


SensorsNameTask::~SensorsNameTask(){
  
  DataHandler::instance().setSensorsNumber();
  for(uint8_t i = 0;i < MAX_CHANNELS; ++i){
    debugValid[i].channel = i;
    debugValid[i].valid = DataHandler::instance().isSensorConnected(i);
    DataHandler::instance().setValid
      (i,DataHandler::instance().isSensorConnected(i)); 
  }
}








