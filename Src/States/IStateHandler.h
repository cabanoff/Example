// IStateHandler.h
#pragma once
#include "main.h"

namespace AppState {
  enum Type {
    NONE,
    INIT,
    POLL_MODE,
    ZERO_SET_MODE,
    RBLAB_MODE,
    TRANSPARENT_MODE,
    ERROR
  };
}

namespace AppEvent {
  enum Type {
    NONE,
    INIT_REQUEST,
    POLL_MODE_REQUEST,
    ZERO_SET_MODE_REQUEST,
    RBLAB_MODE_REQUEST,
    TRANSPARENT_MODE_REQUEST,
    EXIT_TRANSPARENT_MODE,
    EXIT_RBLAB_MODE,
    DEBUG_MODE_REQUEST,
    ERROR_OCCURRED
  };
}

class IStateHandler {
public:
  virtual ~IStateHandler() {}

  virtual void onEnterState(AppState::Type newState) = 0;
};