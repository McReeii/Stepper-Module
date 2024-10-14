#ifndef Comm_h
#define Comm_h

#include "Arduino.h"
#include "Wire.h"

void EspSerial();
void WireSend(uint8_t hold_ms_out, uint8_t click_name, uint8_t click_type_out, uint16_t click, uint8_t wireAddress, uint8_t cmdType);

#endif