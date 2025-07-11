// MediatorObserver.cpp

#include "MediatorObserver.h"
#include "debug_utils.h" 



MediatorObserver::MediatorObserver(IParser& parser) : parser(parser){
  sender = NULL;
}

void MediatorObserver::onMediatorEvent(const volatile uint8_t* data, 
                                   uint16_t size, 
                                   MediatorEvent::Type event){
 
  processMessage(data, size);
}


void MediatorObserver::connectSender(ISender* sender){
  this->sender = sender;
}


bool MediatorObserver::processMessage(const volatile uint8_t* data, 
                                      uint16_t size){
  parse_error_code err = parser.parseFrame(data, size);
  if (err == PARSE_OK) {
    uint8_t* resp = 0;
    uint16_t len = 0;
    parser.answer(resp, len);

    if (resp && len > 0 && sender) {
      uint8_t tempBuf[OBSERVER_BUFFER]; 
      if (len > sizeof(tempBuf)) len = sizeof(tempBuf);
      memcpy(tempBuf, resp, len);
      sender->send(reinterpret_cast<const char*>(tempBuf), len);
    }
  }
  else{
    DEBUG_PRINT("Parse error");
  }
  return (err == PARSE_OK)? true : false;
}