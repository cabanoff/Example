// AlarmGPIO.h

#pragma once

#include "main.h"

class AlarmGPIO {
public:
  static AlarmGPIO& instance() {
    static AlarmGPIO instance;
    return instance;
  }
  void setRelay(uint8_t channel, bool high){
    
    switch(channel){
    case 0: high ? (P2OUT |= BIT5) : (P2OUT &= ~BIT5); break;
    case 1: high ? (P2OUT |= BIT6) : (P2OUT &= ~BIT6); break;
    case 2: high ? (P2OUT |= BIT7) : (P2OUT &= ~BIT7); break;
    }
  }
  
  void setSiren(bool high) { high ? (P1OUT |= BIT4) : (P1OUT &= ~BIT4); }

  void init() {
    static bool initialised = false;
    if(!initialised){
      P2OUT &= ~(BIT5 | BIT6 | BIT7);
      P2DIR |= BIT5 | BIT6 | BIT7;
      P1OUT &= ~BIT4;
      P1DIR |= BIT4;
      initialised = true;
    } 
  }
};