// debug_utils.h
#pragma once
#include "main.h"

#define DEBUG_ENABLED 0

#if DEBUG_ENABLED

#include "Mediator.h"

#define DEBUG_BUF_LENGTH 64

extern char debug_buf[DEBUG_BUF_LENGTH];

#define DEBUG_PRINT(fmt, ...) do { \
    snprintf(debug_buf, sizeof(debug_buf), fmt, ##__VA_ARGS__); \
    Mediator::instance().sendToUART1(debug_buf, strlen(debug_buf)); \
} while(0)

#define DEBUG_PRINTLN(fmt, ...) do { \
    snprintf(debug_buf, sizeof(debug_buf), fmt "\r\n", ##__VA_ARGS__); \
    Mediator::instance().sendToUART1(debug_buf, strlen(debug_buf)); \
} while(0)

#else

#define DEBUG_PRINT(fmt, ...)
#define DEBUG_PRINTLN(fmt, ...)

#endif