// Application.cpp


#include "Application.h"



Application::Application() :
    display(RA8806::instance()),
    rtc(RtcTask::instance()),
    stateTask(NULL),
    stateMachine(this),
    modbusObserver(ModbusParser::instance()),
    rblabObserver(RBLabParser::instance())
{}

void Application::setup(){
  DCOCTL = DCO0|DCO1|DCO2;       // DCO=4MHz
  BCSCTL1 = RSEL0|RSEL1|RSEL2|XTS|XT2OFF|DIVA_0; 
  BCSCTL2 = 0; 
  
  __enable_interrupt();  
  
  display.init();
  Channels::instance().init();
  RtcTask::instance().init();
  Config::instance().init();
  Scheduler::instance().connectTask(&rtc);
  
}

void Application::loop(){
  
  DEBUG_PRINT("Start loop: ");
  
  modbusObserver.connectSender(&UART1Sender);
  Mediator::instance().connectObserver(&modbusObserver,MediatorEvent::RX1);
  
  rblabObserver.connectSender(&UART1Sender);
  Mediator::instance().connectObserver(&rblabObserver,MediatorEvent::RX1);
  
  RBLabParser::instance().setStateMachine(&stateMachine);
 
  while(1){
    stateMachine.update();
    Mediator::instance().run();
    Scheduler::instance().run();
    __low_power_mode_0();  // Wait for interrupt
  }
}


StateTaskBase* Application::createStateTask(AppState::Type newState) {
  switch (newState) {
    case AppState::INIT:
      display.init();
      DEBUG_PRINT(" -> INIT mode: %u bytes ", sizeof(InitStateTask));
      return new InitStateTask(stateMachine, 
                               AppEvent::ZERO_SET_MODE_REQUEST, 
                               display, 
                               &UART0Sender);

    case AppState::ZERO_SET_MODE:
      DEBUG_PRINT(" -> ZERO mode: %u bytes ", sizeof(ZeroStateTask));
      return new ZeroStateTask(stateMachine, 
                               AppEvent::POLL_MODE_REQUEST, 
                               display, 
                               &UART0Sender);
    
    case AppState::POLL_MODE:
      DEBUG_PRINT(" -> POLL mode: %u bytes ", sizeof(PollStateTask));
      return new PollStateTask(stateMachine, 
                               AppEvent::DEBUG_MODE_REQUEST, 
                               display, 
                               &UART0Sender);
      
    case AppState::RBLAB_MODE:
      
      DEBUG_PRINT(" -> RBLab mode: %u bytes ", sizeof(RBLabStateTask));
      return new RBLabStateTask(stateMachine, 
                               AppEvent::POLL_MODE_REQUEST, 
                               display, 
                               NULL);
    default:
      return NULL;
  }
}

void Application::onEnterState(AppState::Type newState) {
  
  if(stateTask){
    delete stateTask;
    stateTask = NULL;
  }
  stateTask = createStateTask(newState);
}



