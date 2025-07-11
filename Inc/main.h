// main.h
#pragma once

#include <io430.h>
#include <stdint.h>  // For uint8_t
#include <stddef.h>  // For size_t
#include <stdio.h>
#include <string.h>

#define VERSION "2.01"

#define CLOCK_KHZ 4000
#define ONE_MS CLOCK_KHZ

#define UARTS_BUFFER_SIZE 255
#define MAX_NAME_LENGTH 30
#define MAX_FORMULA_LENGTH 12

#define MAX_SENSORS 3

#define STATIC_ASSERT(cond, msg)\
typedef char static_assert_##msg[(cond) ? 1 : -1]



