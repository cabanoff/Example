// DataHandler.h
#pragma once

#include "main.h"
#include "modbus_registers_map.h"
#include "Singleton.h"


namespace Ports{
  enum Type{
    NOT_CONNECTED = 0,
    PORT1,
    PORT2,
    PORT3
  };
}

class DataHandler : public Singleton<DataHandler> {
  friend class Singleton<DataHandler>;
public:
  
  void init();
  
  const char* getName(uint8_t channel);
  bool setName(uint8_t channel, const volatile uint8_t* name, uint8_t length);
  
  Ports::Type getPort(uint8_t channel);
  bool setPort(uint8_t channel, Ports::Type port);
  
  bool isSensorInitialized(uint8_t channel);
  bool isSensorConnected(uint8_t channel);
  
  bool setConcentration(uint8_t channel, float value);
  bool setVoltage(uint8_t channel, float value);
  float getConcentration(uint8_t channel);
  float getVoltage(uint8_t channel);
  float getValue(uint8_t channel);

  void setZeroStart(uint8_t channel, bool valid);
  bool getZeroStart(uint8_t channel);
  
  void setZeroFinish(uint8_t channel, bool valid);
  bool getZeroFinish(uint8_t channel);
  
  uint8_t getDecimal(uint8_t channel);
  
  void setValid(uint8_t channel, bool valid);
  bool getValid(uint8_t channel);
  
  void setSensorsNumber();
  uint8_t getSensorsNumber();
  
  void setModBusAddr(uint16_t address);
  uint16_t getModBusAddr();
  
  // Modbus register access methods
  uint16_t getModbusRegister(uint16_t address) const;
  void setModbusRegister(uint16_t address, uint16_t value);
  
private:
  DataHandler();

   
  static const uint8_t MAX_SUBSTANCE_NAME = MAX_NAME_LENGTH;
  static const uint8_t MAX_FORMULA_LEN = MAX_FORMULA_LENGTH;
  static const char* NO_SENSOR;
  static const Ports::Type PORT_INI[MAX_SENSORS];
  
  struct Sensor{
    char substanceName[MAX_SUBSTANCE_NAME];
    char formula[MAX_FORMULA_LEN];
    float voltage;
    float concentration;
    Ports::Type port;
    bool zero_set_started;
    bool zero_set_finished;
  };
  static Sensor sensors[MAX_SENSORS];
  static ModbusRegs modbusRegs;
};


