#include <stdint.h>
#include "Comm.h"
#include "Pins.h"



void EspSerial(){
  Serial.begin(115200);
  while (!Serial) yield();

  Wire.setPins(sda_pin, scl_pin); // Set the I2C pins before begin
  Serial.setDebugOutput(true);
  Wire.begin();
}

  // uint8_t click_out = btn_Click_Out();
  // uint8_t click_name_out = btn_Name_Click_Out();
  // uint8_t click_type_out= btn_Click_Type_Out();
  // uint16_t hold_ms_out= btn_Hold_ms_Out();

void WireSend(uint8_t hold_ms_out, uint8_t click_name_out, uint8_t click_type_out, uint16_t click, uint8_t wireAddress, uint8_t cmdType){ 
//cmdType is for what kind of data is being send i.e. for Button input, Tmc Parameters, Diaplay data
// data is predefined in custom DataSheet 
//Write message to the slave
  Wire.beginTransmission(wireAddress);
  Wire.write(hold_ms_out); // 2 byte int
  Wire.write(click_name_out); // 1 byte bool
  Wire.write(click_type_out); // 1 byte int
  Wire.write(click); // 1 byte int
  Wire.write(cmdType); // 1 byte int
  
  uint8_t error = Wire.endTransmission(true);
  Serial.printf("endTransmission: %u\n", error);
}

void wireRead(){
//Read 16 bytes from the slave
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, 14);
  Serial.printf("requestFrom: %u\n", bytesReceived);
  if((bool)bytesReceived){ //If received more than zero bytes
    uint8_t temp[bytesReceived];
    Wire.readBytes(temp, bytesReceived);
    log_print_buf(temp, bytesReceived);
  }
}