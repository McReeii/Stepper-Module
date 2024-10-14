//ref: https://dronebotworkshop.com/i2c-part-2-build-i2c-sensor/#:~:text=It%20uses%20two%20wires%20for,direction%20at%20any%20given%20moment.

//------>>-------PICO----SLAVE---------||
#include "onrecive.h"

#define I2C_DEV_ADDR 0x55
#define sda_pin 6
#define scl_pin 7


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Wire1.onReceive(onReceive);
  Wire1.onRequest(onRequest111);

  Wire1.setSDA(sda_pin);
  Wire1.setSCL(scl_pin);
  Wire1.begin((uint8_t)I2C_DEV_ADDR);
}

void loop() {
 sleep_ms(100);
}



