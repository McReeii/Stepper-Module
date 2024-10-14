#ifndef EspNow32_h
#define EspNow32_h

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define BOARD_ID 0

void EspNow32_init();
void EspNow32_func();

void EspNowSend();
void getReadings(uint8_t data_1, uint8_t data_2);

uint8_t read_btn_val();
uint8_t read_clk_typ();
bool err_occred();
bool New_Msg_Rcv();
#endif