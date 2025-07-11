// modbus_registers_map.h
#include "main.h"

#define MODBUS_REGS 37

union ModbusRegs {
  struct Regs {
    // Basic device information
    uint16_t slaveAddr;       // Address 00
    uint16_t deviceID;        // Address 01
    // Status register (Address 02)
    uint16_t valid_1 : 1;         // Bit 0
    uint16_t valid_2 : 1;         // Bit 1
    uint16_t valid_3 : 1;          // Bit 2
    uint16_t isThreshold1_1 : 1;    // Bit 3
    uint16_t isThreshold1_2 : 1;    // Bit 4
    uint16_t isThreshold1_3 : 1;    // Bit 5
    uint16_t isThreshold2_1 : 1;    // Bit 6
    uint16_t isThreshold2_2 : 1;     // Bit 7
    uint16_t isThreshold2_3 : 1;     // Bit 8
    uint16_t numSubstances : 2;   // Bits 9-10
    uint16_t ready : 1;            // Bit 11
    uint16_t : 3;                  // Reserved (Bits 12-14)
    uint16_t writeZero : 1;       // Bit 15 (write-only)
    // Measurement data (floats)
    float concentration_1;    // [03–04]
    float concentration_2;    // [05–06]
    float concentration_3;    // [07–08]
    // Units and power status (Address 09)
    uint16_t unit1 : 2;        // Bits 0-1
    uint16_t unit2 : 2;        // Bits 2-3
    uint16_t unit3 : 2;        // Bits 4-5
    uint16_t : 2;             // Reserved (Bits 6-7)
    uint16_t powerPresent : 1; // Bit 8
    uint16_t : 3;             // Reserved (Bits 9-11)
    uint16_t batteryLevel : 4;// Bits 12-15
    // Threshold values (floats)
    float threshold1_1;       // [10–11]
    float threshold1_2;       // [12–13]
    float threshold1_3;       // [14–15]
    float threshold2_1;       // [16–17]
    float threshold2_2;       // [18–19]
    float threshold2_3;       // [20–21]
    // Chemical formulas (8 chars each)
    char formula_1[8];        // [22–25] 
    char formula_2[8];        // [26–29]
    char formula_3[8];        // [30–33]
    // Additional power info
    uint16_t isPower;          // Address 34
    float batteryVoltage;    // [35–36]
  }regs;
  uint16_t rawRegs[MODBUS_REGS]; // Direct register access
  STATIC_ASSERT(sizeof(Regs)/sizeof(uint16_t) == MODBUS_REGS,
              size_of_Regs_struct_must_be_MODBUS_REGS);
};

