// DisplayModbusAddrTask.cpp
#include "DisplayModbusAddrTask.h"

DisplayModbusAddrTask::DisplayModbusAddrTask(IDisplay& display)
  : DisplayTaskBase(display) {}

void DisplayModbusAddrTask::execute() {
  char buffer[6];
  uint16_t modbusAddress = DataHandler::instance().getModBusAddr();
  
  if (modbusAddress > 999) {
      snprintf(buffer, 6, "<--->");
  } else {
      snprintf(buffer, 6, "<%03u>", modbusAddress);
  }
  
  display_.printStr(0,7,buffer);
}