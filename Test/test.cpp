#include "test.h"

#define LED_MASK 0x07  // Example: P5.0–P5.3 used for LEDs



void initClock() {
  DCOCTL = DCO0|DCO1|DCO2;       // DCO=4MHz
  BCSCTL1 = RSEL0|RSEL1|RSEL2|XTS|XT2OFF|DIVA_0; 
  BCSCTL2 = 0; 
}

void setLeds(uint8_t set) {
  P5OUT = (P5OUT & ~LED_MASK) | (~set & LED_MASK);
}

void initLeds() {
  P5DIR |= LED_MASK; // LEDs as output
  setLeds(0x00);     // Turn off all
}



void delay(uint16_t ms) {
  while (ms--) {
    for (volatile uint16_t i = 0; i < 360; ++i)
      __no_operation();
  }
}


#define PAGE_SIZE  256  // AT45DB081E default page size



char writeData[PAGE_SIZE];
char readData[PAGE_SIZE];
uint16_t modbusAddr = 0;
uint16_t blockingTime = 0;
float alarmTreshold_1 = 0.0;
char sensor_name[30];

uint16_t modbusAddr_ = 0;
uint16_t blockingTime_ = 0;
float alarmTreshold_1_ = 0.0;
char sensor_name_[30];




//void saveValueToFlash(uint16_t value);
//uint16_t loadValueFromFlash();

void main( void )
{
    // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  Config config;

  initLeds();
  initClock(); 
  __enable_interrupt();
  
  config.init();
  
  setLeds(0x01); delay(1000);
  
  modbusAddr_ = config.readModbusAddr();
  blockingTime_ = config.readBlockingTime();
  alarmTreshold_1_ = config.readAlarmTreshold_1();
  config.readSensorName(sensor_name_,sizeof(sensor_name),1);
  
  setLeds(0x02); delay(1000);
  
  
  config.writeModbusAddr(11);
  config.writeBlockingTime(840);
  config.writeAlarmTreshold_1(1.1);
  config.writeSensorName("New sensor",1);
  
  
  
  modbusAddr = config.readModbusAddr();
  blockingTime = config.readBlockingTime();
  alarmTreshold_1 = config.readAlarmTreshold_1();
  config.readSensorName(sensor_name,sizeof(sensor_name),1);

  while (1);
}