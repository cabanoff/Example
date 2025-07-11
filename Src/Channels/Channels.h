// Channels.h
#pragma once

#include "main.h"
#include "DataHandler.h"
#include "Singleton.h"


class Channels : public Singleton<Channels> {
  friend class Singleton<Channels>;
public:
  void init();
  void set(uint8_t ch_index);
  void clear();
private:
  Channels();
  uint8_t current_channel_index;
};