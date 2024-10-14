#ifndef EspNow8266_h
#define EspNow8266_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#define BOARD_ID 2


bool EspNowAutoPair();
void EspNow8266_init();
void EspNow8266_func();
void EspNowSend();
void getReadings(uint8_t btn_val, uint8_t clk_typ);

#endif