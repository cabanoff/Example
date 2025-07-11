// Application.h
#pragma once
#include "main.h"
#include "debug_utils.h"
#include "StateMachine.h"

#include "Scheduler.h"
#include "Mediator.h"
#include "RtcTask.h"
#include "Config.h"

#include "MediatorObserver.h"
#include "RBLabParser.h"
#include "ModbusParser.h"

#include "TXSender.h"
#include "LEDTask.h"

#include "InitStateTask.h"
#include "ZeroStateTask.h"
#include "PollStateTask.h"
#include "RBLabStateTask.h"

#include "RA8806.h"
#include "Channels.h"

/**
 * @class Application
 * @brief Main application class responsible for control flow 
 * and mode switching.
 */
class Application : public IStateHandler{
public:
  
  Application();
  /**
   * @brief Set all parameters.
   */
  void setup();
  /**
   * @brief Main application loop.
   */
  void loop();
  
private:
  StateMachine stateMachine;
  IDisplay& display; 
  RtcTask& rtc;

  TX1Sender UART1Sender;
  TX0Sender UART0Sender;

  MediatorObserver modbusObserver, rblabObserver;

  LEDTask leds;

  StateTaskBase* stateTask;
  
   // State handler method
  void onEnterState(AppState::Type newState);
  
  StateTaskBase* createStateTask(AppState::Type newState);
};