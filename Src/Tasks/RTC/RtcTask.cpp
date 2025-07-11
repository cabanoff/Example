//RtcTask.cpp

#include "RtcTask.h"
#include "build_timestamp.h"


// Helper: Check for leap year
static bool isLeap(int year) {
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}



RtcTask::RtcTask() : 
  set_time(0),
  timestamp(0)
  {
  setPeriod(1000);
  }

void RtcTask::execute(){
  timestamp++;
}


void RtcTask::init(){
}
  

// Days in each month
const int daysInMonth[]  = { 31, 28, 31, 30, 31, 30,
                             31, 31, 30, 31, 30, 31 };
const int daysInMonthLeap[] = { 31, 29, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31 };


// Convert string "YY MM DD hh mm ss" to Unix timestamp
void RtcTask::setDateTime(const uint8_t* date_time) {
   // Parse ASCII fields from "YY MM DD hh mm ss"
  uint8_t yy = (date_time[0] - '0') * 10 + (date_time[1] - '0');
  uint8_t mm = (date_time[3] - '0') * 10 + (date_time[4] - '0');
  uint8_t dd = (date_time[6] - '0') * 10 + (date_time[7] - '0');
  uint8_t hh = (date_time[9] - '0') * 10 + (date_time[10] - '0');
  uint8_t mi = (date_time[12] - '0') * 10 + (date_time[13] - '0');
  uint8_t ss = (date_time[15] - '0') * 10 + (date_time[16] - '0');

  // Convert year to full format (assume 2000+)
  uint16_t year = 2000 + yy;

  // Calculate total days since 1970-01-01
  uint32_t days = 0;

  // Add days for all full years before current year
  for (uint16_t y = 1970; y < year; ++y) {
    days += isLeap(y) ? 366 : 365;
  }

  // Add days for full months in current year
  const int* dim = isLeap(year) ? daysInMonthLeap : daysInMonth;
  for (uint8_t m = 1; m < mm; ++m) {
    days += dim[m - 1];
  }

  // Add days passed in current month (subtract 1 because it's zero-based)
  days += dd - 1;

  // Convert total days to seconds
  uint32_t totalSeconds = days * 86400UL;
  totalSeconds += hh * 3600UL;
  totalSeconds += mi * 60UL;
  totalSeconds += ss;
  
  set_time = totalSeconds;
  timestamp = 0;
}

// Convert timestamp to string
void RtcTask::getDateTime(char* dateTime, uint8_t length) {

  uint32_t seconds = timestamp + (set_time ? set_time : BUILD_TIMESTAMP);

  // Extract time
  int sec = seconds % 60;
  seconds /= 60;
  int min = seconds % 60;
  seconds /= 60;
  int hour = seconds % 24;
  seconds /= 24;
  int days = seconds;

  // Compute year
  int year = 1970;
  while (true) {
      int daysInYear = isLeap(year) ? 366 : 365;
      if (days >= daysInYear) {
          days -= daysInYear;
          year++;
      } else {
          break;
      }
  }

  // Compute month and day
  const int* dim = isLeap(year) ? daysInMonthLeap : daysInMonth;
  int month = 0;
  while (days >= dim[month]) {
      days -= dim[month];
      month++;
  }

  int day = days + 1; // 1-based day
  int yy = year % 100; // two-digit year

  // Format string: "DD/MM/YY HH:MM:SS"
  snprintf(dateTime, length, "%02d/%02d/%02d %02d:%02d:%02d",
          day, month + 1, yy, hour, min, sec);
}
  