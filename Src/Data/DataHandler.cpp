// DataHandler.cpp

#include "DataHandler.h"


const char* DataHandler::NO_SENSOR = "NO_SENS";
const Ports::Type DataHandler::PORT_INI[MAX_SENSORS] = {Ports::PORT1,
Ports::PORT2, Ports::PORT3};

DataHandler::Sensor DataHandler::sensors[MAX_SENSORS];
ModbusRegs DataHandler::modbusRegs;

DataHandler::DataHandler(){
     
  init();
  setModBusAddr(1);
  modbusRegs.regs.deviceID = 0;
  modbusRegs.regs.ready = 0;
  const char* formula_1 = "SO2     ";
  memcpy(modbusRegs.regs.formula_1, formula_1, 
         sizeof(modbusRegs.regs.formula_1));

}

void DataHandler::init(){
  
  for(int i = 0; i < MAX_SENSORS; ++i){
    setName(i, (const volatile uint8_t*)NO_SENSOR, 9); 
    setPort(i, PORT_INI[i]);
  }
}


const char* DataHandler::getName(uint8_t channel){
  if(channel < MAX_SENSORS)return sensors[channel].substanceName;
  else return NO_SENSOR;
}


bool DataHandler::setName(uint8_t channel, 
                          const volatile uint8_t* name, 
                          uint8_t length){
  if(length < MAX_SUBSTANCE_NAME && channel < MAX_SENSORS){
    uint8_t i = 0;
    for(; i < length; ++i)
       sensors[channel].substanceName[i] = name[i];
    sensors[channel].substanceName[i] = '\0';
    return true;
  }
  return false;
}


Ports::Type DataHandler::getPort(uint8_t channel){
  return  channel < MAX_SENSORS ? sensors[channel].port 
          : Ports::NOT_CONNECTED;
}


bool DataHandler::isSensorInitialized(uint8_t channel){
  return channel < MAX_SENSORS &&
         sensors[channel].port != Ports::NOT_CONNECTED &&
         strcmp(sensors[channel].substanceName, "None") != 0;
}

bool DataHandler::isSensorConnected(uint8_t channel){
  return channel < MAX_SENSORS && 
         sensors[channel].port != Ports::NOT_CONNECTED;
}

bool DataHandler::setPort(uint8_t channel, Ports::Type port){
  if(channel < MAX_SENSORS){
    sensors[channel].port = port;
    return true;
  }
  else return false;
}

bool DataHandler::setConcentration(uint8_t channel, float value){
  if (channel < MAX_SENSORS){
    sensors[channel].concentration = value;
    if(value < 0) value = 0;
    switch(channel){
    case 0:
      modbusRegs.regs.concentration_1 = value;
      break;
    case 1:
      modbusRegs.regs.concentration_2 = value;
      break;
    case 2:
      modbusRegs.regs.concentration_3 = value;
      break;
    default:
      return false;
    }
    return true;
  }else return false;
}

bool DataHandler::setVoltage(uint8_t channel, float value){
  if (channel < MAX_SENSORS){
    sensors[channel].voltage = value;
    return true;
  }else return false;
  
}

void DataHandler::setModBusAddr(uint16_t address){
  modbusRegs.regs.slaveAddr = address;
}

uint16_t DataHandler::getModBusAddr(){
  return modbusRegs.regs.slaveAddr;
}

void DataHandler::setZeroStart(uint8_t channel, bool valid){
  if (channel < MAX_SENSORS)
    sensors[channel].zero_set_started = valid;
}

void DataHandler::setZeroFinish(uint8_t channel, bool valid){
  if (channel < MAX_SENSORS)
   sensors[channel].zero_set_finished = valid;
}

bool DataHandler::getZeroStart(uint8_t channel){
  if (channel < MAX_SENSORS)
    return sensors[channel].zero_set_started;
  return false;
}

bool DataHandler::getZeroFinish(uint8_t channel){
  if (channel < MAX_SENSORS)
    return sensors[channel].zero_set_finished;
  return false;
}


float DataHandler::getValue(uint8_t channel){
  //to do: return voltage or concentration
  if(strcmp(sensors[channel].substanceName,"None") == 0 ){
    return getVoltage(channel);
  }
  else return getConcentration(channel); 
}

float DataHandler::getConcentration(uint8_t channel){
  
  switch(channel){
  case 0:
    return modbusRegs.regs.concentration_1;
  case 1:
    return modbusRegs.regs.concentration_2;
  case 2:
    return modbusRegs.regs.concentration_3;
  }
  return 0.0;
}

float DataHandler::getVoltage(uint8_t channel){
  if (channel < MAX_SENSORS)
    return sensors[channel].voltage;
  return 0.0;
}

//how many decimal places to display
uint8_t DataHandler::getDecimal(uint8_t channel){
  return 2;
}

void DataHandler::setValid(uint8_t channel, bool valid){
  switch(channel){
  case 0:
    modbusRegs.regs.valid_1 = valid;
    break;
  case 1:
    modbusRegs.regs.valid_2 = valid;
    break;
  case 2:
    modbusRegs.regs.valid_3 = valid;
    break;
  }
}

bool DataHandler::getValid(uint8_t channel){
  switch(channel){
  case 0:
    return modbusRegs.regs.valid_1;
  case 1:
    return modbusRegs.regs.valid_2;
  case 2:
    return modbusRegs.regs.valid_3;
  }
  return true;
  
}


void DataHandler::setSensorsNumber(){
  uint8_t number = 0;
  for(uint8_t i = 0; i < MAX_SENSORS; ++i){
    if((strcmp(sensors[i].substanceName,"None") != 0 )&&
      (strcmp(sensors[i].substanceName,"NO_SENS") != 0 )) number++;
  }
  modbusRegs.regs.numSubstances = number;
}

uint8_t DataHandler::getSensorsNumber(){
  return modbusRegs.regs.numSubstances;
}


uint16_t DataHandler::getModbusRegister(uint16_t address) const {
  return (address < MODBUS_REGS) ? modbusRegs.rawRegs[address] : 0;
}

void DataHandler::setModbusRegister(uint16_t address, uint16_t value) {
  if (address < MODBUS_REGS) {
    modbusRegs.rawRegs[address] = value;
  }
}

