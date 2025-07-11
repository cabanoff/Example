// Flash.cpp
#pragma once
#include "main.h"

// Define the flash segment address (Segment D)
#define FLASH_SEGMENT_ADDR  ((uint16_t*)0x1000)
#define SEGMENT_SIZE        64  // 64 bytes

void flashEraseSegment(uint16_t* address) {
    __disable_interrupt(); 
    FCTL2 = FWKEY + FSSEL0 + FN1;       // MCLK/3 for timing
    FCTL3 = FWKEY;                      // Clear LOCK
    FCTL1 = FWKEY + ERASE;              // Set erase bit
    *address = 0;                       // Dummy write to trigger erase
    while (FCTL3 & BUSY);               // Wait for erase to complete
    FCTL1 = FWKEY;                      // Clear erase bit
    FCTL3 = FWKEY + LOCK;               // Re-lock flash
    __enable_interrupt(); 
}

void flashWriteWord(uint16_t* address, uint16_t value) {
    __disable_interrupt(); 
    FCTL2 = FWKEY + FSSEL0 + FN1;       // MCLK/3
    FCTL3 = FWKEY;                      // Unlock flash
    FCTL1 = FWKEY + WRT;                // Enable write
    *address = value;                   // Write value
    while (FCTL3 & BUSY);               // Wait for completion
    FCTL1 = FWKEY;                      // Disable write
    FCTL3 = FWKEY + LOCK;               // Re-lock flash
    __enable_interrupt(); 
}

uint16_t flashReadWord(const uint16_t* address) {
    return *address;
}

void saveValueToFlash(uint16_t value) {
    flashEraseSegment(FLASH_SEGMENT_ADDR);       // Erase flash
    flashWriteWord(FLASH_SEGMENT_ADDR, value);   // Write one word
}

uint16_t loadValueFromFlash() {
    return flashReadWord(FLASH_SEGMENT_ADDR);    // Read back
}