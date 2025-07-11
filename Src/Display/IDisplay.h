// IDisplay.h
#pragma once
#include "main.h"

// Error codes used across display out
typedef enum {
    DISPLAY_OUTPUT_OK = 0,
    EXCEEDS_SIZE,
    SPECIAL_SYMBOL
} display_error_code;


class IDisplay {
public:
  virtual ~IDisplay() {}

  virtual void init() = 0;
  virtual void clrscr() = 0;
  virtual void printAcc(uint8_t level) = 0;

  virtual display_error_code printStr(uint8_t row, 
                                      uint8_t col, 
                                      const char* s) = 0;
  virtual display_error_code printnStr(uint8_t row, 
                                       uint8_t col, 
                                       const char* s, 
                                       uint8_t len = 1) = 0;
};