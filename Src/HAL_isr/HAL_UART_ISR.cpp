// HAL_UART_ISR.cpp

#include "HAL_UART1.h"
#include "HAL_UART0.h"


#pragma vector=USART1RX_VECTOR
__interrupt void UART1_RX_ISR(void) {  
  char c = U1RXBUF;
  HAL_UART1::instance->handleRXInterrupt(c);
}

#pragma vector=USART1TX_VECTOR
__interrupt void UART1_TX_ISR(void){
  HAL_UART1::instance->handleTXInterrupt();
}

#pragma vector=USART0RX_VECTOR
__interrupt void UART0_RX_ISR(void){
  char c = U0RXBUF;
  HAL_UART0::instance->handleRXInterrupt(c);
}

#pragma vector=USART0TX_VECTOR
__interrupt void UART0_TX_ISR(void){
  HAL_UART0::instance->handleTXInterrupt();
}
