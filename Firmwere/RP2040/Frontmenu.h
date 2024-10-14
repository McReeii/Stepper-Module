#ifndef Frontmenu_h
#define Frontmenu_h

#include "Arduino.h"
#include "U8g2lib.h"
#include "Pins.h"

extern bool MotorStop;
void Oled_init();
void MenuSystem();

int currentscreen();
int curruntMenuItem();

void Settings_menu();

void Stroking_u8g();
void continuous_Run_u8g();


#endif