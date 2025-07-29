// StateMachine.cpp
#include "StateMachine.h"

StateMachine::StateMachine(IStateHandler* handler)
  : current_state(AppState::NONE),
    next_state(AppState::INIT),
    before_rblab_state(AppState::ZERO_SET_MODE),
    event(AppEvent::INIT_REQUEST),
    handler(handler)
    {}

void StateMachine::setEvent(AppEvent::Type e) {
  event = e;
}

AppState::Type StateMachine::getState() const {
  return current_state;
}

void StateMachine::update() {
  
  next_state = current_state;
  
  switch (event){
    
    case AppEvent::INIT_REQUEST:
      next_state = AppState::INIT;
      break;
    case AppEvent::ZERO_SET_MODE_REQUEST:
      next_state = AppState::ZERO_SET_MODE;
      break;
    case AppEvent::POLL_MODE_REQUEST:
      next_state = AppState::POLL_MODE;
      before_rblab_state = AppState::POLL_MODE;
      break;
    case AppEvent::DEBUG_MODE_REQUEST:
      next_state = AppState::INIT;
      break;
    case AppEvent::RBLAB_MODE_REQUEST:
      next_state = AppState::RBLAB_MODE;
      break;
    case AppEvent::TRANSPARENT_MODE_REQUEST:
      next_state = AppState::TRANSPARENT_MODE;
      break;
    case AppEvent::EXIT_TRANSPARENT_MODE:
      if(current_state == AppState::TRANSPARENT_MODE){
        next_state = AppState::RBLAB_MODE;
      }
      break;
    case AppEvent::EXIT_RBLAB_MODE:
      if(current_state == AppState::RBLAB_MODE ||
         current_state == AppState::TRANSPARENT_MODE){
        next_state = before_rblab_state;
      }
      break;
    default:
      break;
    
  }

  if (next_state != current_state)
    transition();

  event = AppEvent::NONE;
}

void StateMachine::transition() {

  current_state = next_state;

  if (handler) handler->onEnterState(current_state);
    
}
