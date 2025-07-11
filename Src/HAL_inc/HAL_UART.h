// UART.h
#pragma once
#include "main.h"

/**
 * @class HAL_UART
 * @brief Base class for UART communication.
 */
class HAL_UART {
public:
  virtual ~HAL_UART() {}
  /**
   * @brief initialize UART 19200 baud .
   * @param data Pointer to the data to be sent.
   * @param length Length of the data to be sent.
   */
  virtual void init() = 0;
  /**
   * @brief Sends data through UART.
   * @param data Pointer to the data to be sent.
   * @param length Length of the data to be sent.
   */
  virtual bool send(const char* data, size_t length) = 0;

  
  virtual inline void handleRXInterrupt(char in_char) = 0;
  virtual inline void handleTXInterrupt(void);
};
