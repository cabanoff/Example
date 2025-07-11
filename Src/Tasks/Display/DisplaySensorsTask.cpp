// DisplaySensorsTask.cpp
#include "DisplaySensorsTask.h"

/**
*  responsible for displaying stuff name and value  
*
*/

DisplaySensorsTask::DisplaySensorsTask(IDisplay& display)
  : DisplayTaskBase(display) {}

void DisplaySensorsTask::execute() {
  for (int i = 0; i < MAX_SENSORS; ++i) {
    if (DataHandler::instance().isSensorConnected(i)) {
      const char* name = DataHandler::instance().getName(i);
      if (name != NULL) display_.printStr(1 + i * 3, 1, name);
      if(!DataHandler::instance().getValid(i)){
        display_.printStr(2 + i * 3, 1, "         Not valid");
      }
      else{
        float value = DataHandler::instance().getValue(i);
        uint8_t decimal = DataHandler::instance().getDecimal(i);
        const char* units = Config::instance().readUnits(i);
        const char* displayValue = valueToDisplay(value, decimal, units);
        display_.printStr(2 + i * 3, 1, displayValue);
      }     
    }
  }
}


uint8_t DisplaySensorsTask::length(const char* str) {
  if (!str) return 0;
  return (uint8_t)strlen(str);
}

const char* DisplaySensorsTask::valueToDisplay(float value, 
                                               uint8_t decimal,
                                               const char* units) {
  static char displayBuffer[19]; // 18 characters + null terminator
    
  if (decimal > 6) decimal = 6;

  // First format the numeric value with specified decimals
  char numberPart[19];
  snprintf(numberPart, sizeof(numberPart), "%.*f", decimal, value);

  // Combine number and units with potential truncation
  char combined[19];
  snprintf(combined, sizeof(combined), "%s%s", numberPart, units);

  // Right-align the combined result in 18-character field
  snprintf(displayBuffer, sizeof(displayBuffer), "%18s", combined);

  // Ensure proper termination (snprintf might not truncate exactly)
  displayBuffer[18] = '\0';

  return displayBuffer;
}

