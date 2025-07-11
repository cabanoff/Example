// DisplayGPIO.h

#pragma once

#include "main.h"

class DisplayGPIO {
public:
  static DisplayGPIO& instance() {
    static DisplayGPIO instance;
    return instance;
  }
  void setData(uint8_t value) { P4OUT = value; }
  void setRS(bool high) { high ? (P5OUT |= BIT5) : (P5OUT &= ~BIT5); }
  void setRW(bool high) { high ? (P5OUT |= BIT6) : (P5OUT &= ~BIT6); }
  void setE(bool high)  { high ? (P5OUT |= BIT7) : (P5OUT &= ~BIT7); }

  void init() {
      P4DIR = 0xFF;      // DB0-DB7 as output
      P5DIR |= BIT5 | BIT6 | BIT7; // RS, RW, E as output
  }
};