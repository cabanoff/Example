// HAL_UART0.cpp
#include "HAL_UART0.h"


HAL_UART0* HAL_UART0::instance = NULL;
volatile HAL_UART0::Buffer HAL_UART0::tx = {{0}, 0, 0};

HAL_UART0::HAL_UART0()
: rxCallback(NULL), rxContext(NULL),txCallback(NULL), txContext(NULL)
{
  instance = this;
  init();
}

void HAL_UART0::init(void){
  P3SEL_bit.P4 = 1;
  P3SEL_bit.P5 = 1;
  
//  U0CTL_bit.SWRST = 1;
//  U0CTL_bit.CHAR = 1;
//  U0TCTL_bit.TXEPT = 1;
//  U0TCTL_bit.SSEL0 = 1;
//  U0TCTL_bit.SSEL1 = 1;
//  
//  U0BR0 = 9;
//  U0BR1 = 1;
//  U0MCTL = 0;
//  
//  ME1_bit.URXE0 = 1;
//  ME1_bit.UTXE0 = 1;
//  
//
//  U0CTL_bit.SWRST = 0;
//  //IE1_bit.UTXIE0 = 1;
//  IE1_bit.URXIE0 = 1;
  U0CTL = CHAR;
  U0TCTL = 0|TXEPT|SSEL0;
  U0RCTL = 0;
  U0BR0 = 208; //для 4800 и 1MHz  для 19200 и 4MHz
  U0BR1 = 0x00;
  U0MCTL=0x00;   // no modulation
  ME1 = 0|UTXE0|URXE0;             // enable transmit/receive  UART0
  U0CTL &= ~SWRST;                 // Initialize USART state machine
  IE1 |= URXIE0;                   // Enable USART0 RX interrupt
  
}

bool HAL_UART0::isTXReady(){
  return !tx.index;
}

bool HAL_UART0::send(const char* data, size_t length){
  const uint8_t utxie_saved = IE1_bit.UTXIE0;
  IE1_bit.UTXIE0 = 0;   // critical section: protect tx buffer

  bool canSend = isTXReady() && (length < sizeof(tx.buffer));

  if (canSend) {
    tx.end = length;
    tx.index = 0;

    // Copy data to the transmission buffer
    for (size_t i = 0; i < tx.end; ++i) {
      tx.buffer[i] = data[i];
    }

    U0TXBUF = tx.buffer[tx.index++];
    IE1_bit.UTXIE0 = 1; // re-enable TX interrupt to handle rest
  }

  // Restore previous interrupt enable state if we didn't already
  if (!canSend) {
    IE1_bit.UTXIE0 = utxie_saved;
  }

  return canSend;
}


 //Callback connect/disconnect:
void HAL_UART0::connectRxCallback(RxCallback cb, void* ctx) {
  rxCallback = cb;
  rxContext = ctx;
}

void HAL_UART0::disconnectRxCallback() {
  rxCallback = 0;
  rxContext = 0;
}

void HAL_UART0::connectTxCallback(TxCallback cb, void* ctx) {
  txCallback = cb;
  txContext = ctx;
}

void HAL_UART0::disconnectTxCallback() {
  txCallback = 0;
  txContext = 0;
}

inline void HAL_UART0::handleRXInterrupt(char in_char){
  if (rxCallback)rxCallback(in_char, rxContext);
}

inline void HAL_UART0::handleTXInterrupt(void){
  const uint16_t tx_end = tx.end;
  if (tx.index < tx_end) U0TXBUF = tx.buffer[tx.index++]; 
  else {
    IE1_bit.UTXIE0 = 0;
    tx.index = tx.end = 0;
    if (txCallback)txCallback(txContext);
  }
}



  