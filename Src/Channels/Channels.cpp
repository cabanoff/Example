// Channels.cpp
#include "Channels.h"



Channels::Channels(): current_channel_index(0){}

void Channels::init(){
  
  P6DIR |= BIT0 | BIT1 | BIT2; // SE1, SE2, SE3 as output  
  P6OUT |= BIT0 | BIT1 | BIT2; // SE1, SE2, SE3 = 1 
  
  P2DIR_bit.P2 = 1;//BSLRX
  P2OUT_bit.P2 = 1;
  P1DIR_bit.P1 = 1;//BSLTX
  P1OUT_bit.P1 = 0;
  
}


void  Channels::set(uint8_t ch_index){
  current_channel_index = ch_index;

  switch(ch_index){  
    //case Ports::PORT1:
    case 0: 
      P6OUT_bit.P0 = 0, P6OUT_bit.P1 = 1, P6OUT_bit.P2 = 1;
      P5OUT_bit.P0 = 0, P5OUT_bit.P1 = 1, P5OUT_bit.P2 = 1;//fd
      break;
    //case Ports::PORT2:
    case 1:
      P6OUT_bit.P0 = 1, P6OUT_bit.P1 = 0, P6OUT_bit.P2 = 1; 
      P5OUT_bit.P0 = 1, P5OUT_bit.P1 = 0, P5OUT_bit.P2 = 1;//fd
      break;
    //case Ports::PORT3:
    case 2:
      P6OUT_bit.P0 = 1, P6OUT_bit.P1 = 1, P6OUT_bit.P2 = 0; 
      P5OUT_bit.P0 = 1, P5OUT_bit.P1 = 1, P5OUT_bit.P2 = 0; //fd
      break;
    default:
      clear();

  }
}


void Channels::clear() {
  P6OUT_bit.P0 = 1; P6OUT_bit.P1 = 1; P6OUT_bit.P2 = 1;
  P5OUT_bit.P0 = 1; P5OUT_bit.P1 = 1; P5OUT_bit.P2 = 1; //fd
}
