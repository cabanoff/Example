// ISender.h

#pragma once

#include "main.h"


class ISender {
public:
  virtual ~ISender() {}
  virtual bool send(const char* data, size_t length) = 0;
};
