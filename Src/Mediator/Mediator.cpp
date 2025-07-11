//Mediator.cpp

#include "Mediator.h"




volatile Mediator::Buffer Mediator::rx1 = {{0},0}; 
volatile Mediator::Buffer Mediator::in_hold1 = {{0},0}; 

volatile Mediator::Buffer Mediator::rx0 = {{0},0};
volatile Mediator::Buffer Mediator::in_hold0 = {{0},0};


Mediator::Mediator(): 
UARTsTransMode(false),
uart1(),
uart0(),
timerA(TIMER_A_TIC100US)
{
  //instance = this;
  uart1.connectRxCallback(staticRxHandler1, this);
  timerA.connectUARTTimeOutCallback(HAL_TimerA::UART1, 
                                    staticTimeoutHandler1,this);
  uart0.connectRxCallback(staticRxHandler0, this);
  timerA.connectUARTTimeOutCallback(HAL_TimerA::UART0, 
                                    staticTimeoutHandler0, this);
  
  timerA.connectUARTTimeOutCallback(HAL_TimerA::TRANS, 
                                    staticTimeoutHandlerTrans, this);
  
  uart0.connectTxCallback(staticTxHandler0, this);
  uart1.connectTxCallback(staticTxHandler1, this);
  
  for(int i = 0; i < MediatorEvent::COUNT; ++i){
    eventReady[i] = false;
    for(int j = 0; j < MAX_OBSERVERS_PER_EVENT; ++j){
      eventObservers[i][j] = NULL;
    }
  }
}

void Mediator::staticTimeoutHandlerTrans(void* ctx){
  static_cast<Mediator*>(ctx)->onTransTimeout();
}

void Mediator::staticRxHandler1(char byte, void* ctx){
  static_cast<Mediator*>(ctx)->onUart1ByteReceived(byte);
}

void Mediator::staticTimeoutHandler1(void* ctx){
  static_cast<Mediator*>(ctx)->onUart1Timeout();
}

void Mediator::staticRxHandler0(char byte, void* ctx){
  static_cast<Mediator*>(ctx)->onUart0ByteReceived(byte);
}

void Mediator::staticTimeoutHandler0(void* ctx) {
  static_cast<Mediator*>(ctx)->onUart0Timeout();
}

void Mediator::staticTxHandler0(void* ctx) {
  static_cast<Mediator*>(ctx)->onUart0TxEnd();
}

void Mediator::staticTxHandler1(void* ctx) {
  static_cast<Mediator*>(ctx)->onUart1TxEnd();
}

void Mediator::onUart1ByteReceived(char byte){
  rx1.buffer[rx1.index] = byte;
  rx1.index = (rx1.index + 1) % sizeof(rx1.buffer);
  timerA.set_uart_timeout_100us(timerA.UART1,TIMEOUT_2MS); 
  if(UARTsTransMode){
    if((byte == 0x04)||(byte == 0x19)){ //to do: make return from transMode bytes
      setNormalMode();
      return;
    }
    TXBUF0 = byte;
    if(byte == 0)uart1.out(true); //set RS485 in TX
  }
}

void Mediator::onUart0ByteReceived(char byte){
  
  if(UARTsTransMode){
    TXBUF1 = byte;
    timerA.set_uart_timeout_100us(timerA.TRANS,TIMEOUT_2MS);
    return;
  } 
  rx0.buffer[rx0.index] = byte;
  rx0.index = (rx0.index + 1) % sizeof(rx0.buffer);
  timerA.set_uart_timeout_100us(timerA.UART0,TIMEOUT_2MS);
}


void Mediator::run() {
  
  for (int event = 0; event < MediatorEvent::COUNT; ++event){
    if (eventReady[event]) {
      eventReady[event] = false;

      const volatile uint8_t* buffer = 0;
      uint16_t size = 0;

      switch (event) {
        case MediatorEvent::RX0:
          buffer = in_hold0.buffer;
          size = in_hold0.index;
          break;
        case MediatorEvent::RX1:
          buffer = in_hold1.buffer;
          size = in_hold1.index;
          break;
        default:
          break;
      }

      for (uint8_t i = 0; i < MAX_OBSERVERS_PER_EVENT; ++i) 
        if (eventObservers[event][i])
          eventObservers[event][i]-> 
            onMediatorEvent(buffer, size, 
                            static_cast<MediatorEvent::Type>(event));
    }
  }
}

void Mediator::onUart1Timeout(){
  if (rx1.index > 0 ) {
    for (int i = 0; i < rx1.index; ++i){
      in_hold1.buffer[i] = rx1.buffer[i];
      rx1.buffer[i] = 0; 
    }
    in_hold1.index = rx1.index;
    eventReady[MediatorEvent::RX1] = true;
    rx1.index = 0;
    
    if (UARTsTransMode){
      // end message from PC, switch to RS485 tx
      //set timer to wait answer from a sensor
      timerA.set_uart_timeout_100us(timerA.TRANS,TIMEOUT_2MS * 80); 
    }
  }
  
}

void Mediator::onUart0Timeout(){
  if (rx0.index > 0) {
    for (int i = 0; i < rx0.index; ++i){
      in_hold0.buffer[i] = rx0.buffer[i];
      rx0.buffer[i] = 0; 
    }
    in_hold0.index = rx0.index;
    eventReady[MediatorEvent::RX0] = true;
    rx0.index = 0;
    
    //a message end from the sensor, switch to RS485 rx
    if(UARTsTransMode) uart1.out(false); 
  }
  
}

void Mediator::onTransTimeout(){
  uart1.out(false);
}

void Mediator::onUart0TxEnd(){
  eventReady[MediatorEvent::TX0] = true;
}

void Mediator::onUart1TxEnd(){
  eventReady[MediatorEvent::TX1] = true;
  //time out to switch RS485 to RX mode
  timerA.set_uart_timeout_100us(timerA.TRANS,TIMEOUT_1MS);  
}



observer_error_code Mediator::connectObserver(IMediatorObserver* observer, 
                                              MediatorEvent::Type event){
  if (!observer || event >= MediatorEvent::COUNT) 
    return OBSERVER_INVALID_PARAM;

  // Check if observer already connected for this event
  for (uint8_t i = 0; i < MAX_OBSERVERS_PER_EVENT; ++i) {
    if (eventObservers[event][i] == observer) {
      return OBSERVER_ALREADY_ACTIVE;
    }
  }

  // Find an empty slot
  for (uint8_t i = 0; i < MAX_OBSERVERS_PER_EVENT; ++i) {
    if (eventObservers[event][i] == NULL) {
      eventObservers[event][i] = observer;
      return OBSERVER_OK;
    }
  }

  // No empty slots
  return OBSERVER_HANDLER_FULL;
}

observer_error_code Mediator::disconnectObserver
(IMediatorObserver* observer){
  if (!observer) return OBSERVER_INVALID_PARAM;

  for(int e = 0; e < MediatorEvent::COUNT; ++e){
    for(int i = 0; i < MAX_OBSERVERS_PER_EVENT; ++i){
      if(eventObservers[e][i] == observer){
        eventObservers[e][i] = 0;
        return OBSERVER_OK;
      }
    }
  }
  return OBSERVER_NOT_ACTIVE;
}

bool Mediator::sendToUART1(const char* data, size_t length){
  return uart1.send(data, length);
}

bool Mediator::sendToUART0(const char* data, size_t length){
  return uart0.send(data, length);
}

void Mediator::setTransMode(){
  UARTsTransMode = true;
 // timerA.connectUARTTimeOutCallback(HAL_TimerA::TRANS, 
 //                                   staticTimeoutHandlerTrans, this);

}

void Mediator::setNormalMode(){
  UARTsTransMode = false;
 // timerA.disconnectUARTTimeOutCallback(HAL_TimerA::TRANS);
  timerA.set_uart_timeout_100us(timerA.TRANS,0);  
}