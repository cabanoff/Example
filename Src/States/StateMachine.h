// StateMachine.h
#pragma once
#include "IStateHandler.h"

class StateMachine {
public:
  StateMachine(IStateHandler* handler);

  void setEvent(AppEvent::Type e);
  AppState::Type getState() const;
  void update(); // Call in loop

private:
  AppState::Type current_state;
  AppState::Type next_state;
  AppState::Type last_state;
  AppEvent::Type event;
  IStateHandler* handler;

  void transition();
  
  void setStateDebugEnterTransMode();
  void setStateDebugExitTransMode();
};