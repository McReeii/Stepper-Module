//ref: https://dronebotworkshop.com/i2c-part-2-build-i2c-sensor/#:~:text=It%20uses%20two%20wires%20for,direction%20at%20any%20given%20moment.

//----||---------ESP----MASTER--------->>
#include "Wire.h"

#define sda_pin 4
#define scl_pin 5

#define I2C_DEV_ADDR 0x55

uint8_t byt1 = 0; // 1 bytes
uint16_t byt2 = 0; // 2 bytes
uint32_t byt4 = 0; // 4 bytes
uint64_t byt8 = 0; // 8 bytes

bool x = true; // 1 bytes
int y = 254; // 1 bytes

void setup() {
  Serial.begin(115200);
  Wire.setPins(sda_pin, scl_pin); // Set the I2C pins before begin
  Serial.setDebugOutput(true);
  Wire.begin();
}

void loop() {
  delay(100);

  //Write message to the slave
  Wire.beginTransmission(I2C_DEV_ADDR);
  Wire.printf("Hello World!"); // 12 bytes
  Wire.write(x); // 1 byte bool
  Wire.write(y); // 1 byte int

  uint8_t error = Wire.endTransmission(true);
  Serial.printf("endTransmission: %u\n", error);
  
  //Read 16 bytes from the slave
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, 14);
  Serial.printf("requestFrom: %u\n", bytesReceived);
  if((bool)bytesReceived){ //If received more than zero bytes
    uint8_t temp[bytesReceived];
    Wire.readBytes(temp, bytesReceived);
    log_print_buf(temp, bytesReceived);
  }
}
