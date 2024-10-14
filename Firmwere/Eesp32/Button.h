#ifndef Button_h
#define Button_h

#include "Arduino.h"
#include "Pins.h"
#include "Comm.h"

#define DBLCLK_DELAY 100

void click();
void Button_init();
void Button_func();
bool btn_Trigger_Out();

uint8_t btn_LongPress_Out();
uint8_t btn_Click_Out();
uint8_t btn_Name_Click_Out();
uint8_t btn_Click_Type_Out();
uint16_t btn_Hold_ms_Out();

#endif