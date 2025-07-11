// HAL_UART1.cpp
#include "HAL_UART1.h"


HAL_UART1* HAL_UART1::instance = NULL;
//volatile HAL_UART1::Buffer HAL_UART1::tx = {{0}, 0, 0};
volatile HAL_UART1::RingBuffer HAL_UART1::tx;


HAL_UART1::HAL_UART1()
: rxCallback(NULL), rxContext(NULL), txCallback(NULL), txContext(NULL)
{
  instance = this;
  init();
  out(false);
  tx.reset(); //new
}

void HAL_UART1::init(void){
  P2DIR_bit.P0 = 1;
  P3SEL_bit.P6 = 1;
  P3SEL_bit.P7 = 1;
  
//  U1CTL_bit.SWRST = 1;
//  U1CTL_bit.CHAR = 1;
//  U1TCTL_bit.TXEPT = 1;
//  U1TCTL_bit.SSEL0 = 1;
//  U1TCTL_bit.SSEL1 = 1;
//
//  U1BR0 = 9;
//  U1BR1 = 1;
//  U1MCTL = 0;
//
//  ME2_bit.URXE1 = 1;
//  ME2_bit.UTXE1 = 1 ;
// 
//  U1CTL_bit.SWRST = 0;
//  IE2_bit.UTXIE1 = 0; 
//  IE2_bit.URXIE1 = 1;
  U1CTL=CHAR;
  U1TCTL = 0|TXEPT|SSEL0;
  U1RCTL = 0;
  U1BR0 = 208;
  U1BR1 = 0x00;
  U1MCTL= 0x00;     // no modulation
  ME2 = 0|UTXE1|URXE1;             // enable transmit/receive  UART1
  U1CTL &= ~SWRST;                 // Initialize USART state machine
  IE2 |= URXIE1;                    //  Enable USART1 RX interrupt
}

void HAL_UART1::out(bool on){
  if(on){
    P2OUT_bit.P0 = 1;
    flOut = true;
    P5OUT_bit.P2 = 0; //fd
  }
  else{
    P2OUT_bit.P0 = 0;
    flOut = false;
    P5OUT_bit.P2 = 1; //fd
  }
}


bool HAL_UART1::isTXReady() {
  return tx.isEmpty();
}


//bool HAL_UART1::isTXReady(){
//  return !tx.index;
//}

bool HAL_UART1::send(const char* data, size_t length) {
  bool wasEmpty = tx.isEmpty();
  
  const uint8_t utxie_saved = IE2_bit.UTXIE1;
  IE2_bit.UTXIE1 = 0;         // Critical section: protect tx buffer
  
  for (size_t i = 0; i < length; ++i) {
    uint16_t next = (tx.head + 1) % BUFFER_SIZE;

    if (next == tx.tail) {
      // Buffer full – restore interrupt and abort
      IE2_bit.UTXIE1 = utxie_saved;
      return false;
    }

    tx.buffer[tx.head] = data[i];
    tx.head = next;
  }
  
  IE2_bit.UTXIE1 = utxie_saved;  // End critical section

  if (wasEmpty) {
    startTransmission();
  }

  return true;
}


//bool HAL_UART1::send(const char* data, size_t length){
//
//    // Check if the UART is currently busy transmitting
//  if (isTXReady() && (length  < sizeof(tx.buffer))) {
//    // UART1 TX buffer is empty, start transmission immediately
//    tx.end = length; 
//    // Copy data to the transmission buffer
//    for (size_t i = 0; i < tx.end; ++i) {
//        tx.buffer[i] = data[i];
//    }
//   
//    if(!flOut){
//      out(true);
//      __delay_cycles(ONE_MS); //1 ms
//    }
//
//    // Start by sending the first byte
//
//    U1TXBUF = tx.buffer[tx.index];   
//    tx.index++;
//    // Enable UART1 TX interrupt to handle subsequent bytes
//    IE2_bit.UTXIE1 = 1; 
//    return true;
//  } else {
//    // UART1 is busy, store the data for later
//    // Optionally handle buffering or error
//    return false; 
//  }
//}

void HAL_UART1::startTransmission() {
  if (tx.isEmpty()) return;
  
  // Critical section: prepare first byte safely
  const uint8_t utxie_saved = IE2_bit.UTXIE1;
  IE2_bit.UTXIE1 = 0;

  if (!flOut) out(true);

  U1TXBUF = tx.buffer[tx.tail];
  tx.tail = (tx.tail + 1) % BUFFER_SIZE;

  IE2_bit.UTXIE1 = 1;  // Enable TX interrupt
}


  //Callback connect/disconnect:
void HAL_UART1::connectRxCallback(RxCallback cb, void* ctx) {
  rxCallback = cb;
  rxContext = ctx;
}

void HAL_UART1::disconnectRxCallback() {
  rxCallback = 0;
  rxContext = 0;
}

void HAL_UART1::connectTxCallback(TxCallback cb, void* ctx) {
  txCallback = cb;
  txContext = ctx;
}

void HAL_UART1::disconnectTxCallback() {
  txCallback = 0;
  txContext = 0;
}

inline void HAL_UART1::handleRXInterrupt(char in_char){
  if (rxCallback)rxCallback(in_char, rxContext);
}


inline void HAL_UART1::handleTXInterrupt() {
  uint16_t localTail = tx.tail;
  uint16_t localHead = tx.head;

  if (localTail != localHead) {
    U1TXBUF = tx.buffer[localTail];
    tx.tail = (localTail + 1) % BUFFER_SIZE;
  } else {
    IE2_bit.UTXIE1 = 0; // Disable TX interrupt
    //__delay_cycles(ONE_MS);
    //out(false);
    if (txCallback) txCallback(txContext);
  }
}


//inline void HAL_UART1::handleTXInterrupt(void){
//  const uint16_t tx_end = tx.end;
//  if (tx.index < tx_end){
//    U1TXBUF = tx.buffer[tx.index];    
//    tx.index++;
//  } else {
//    IE2_bit.UTXIE1 = 0;
//    tx.index = 0;
//    tx.end = 0;
//    __delay_cycles(ONE_MS/2); //0.5 ms
//    out(false);
//    if (txCallback)txCallback(txContext);
//  }
//}

