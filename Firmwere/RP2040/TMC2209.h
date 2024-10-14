#ifndef TMC2209_h
#define TMC2209_h

#include "Arduino.h"
#include "Wire.h"
#include "Frontmenu.h"
#include "Button.h"
#include "Pins.h"

void TmcSpeed(int Vactual);
void TmcStop();

void Tmc2209_init();
void Homming();
void backlash();

void UserInput();
void continuous_Run();
void Stroking();
// void continuous_Run(int TMC_Speed);
// void Stroking(int St_Speed , int timeGap);
// void image360_u8g();
// void Settings_menu();

int tmcSpeedOut(int TMC_Speed);
int tmcDirOut();
int tmcVrefOut();
bool HallOut();
int Speed_UserInput_Out();
int timeDelay_UserInput_Out();

#endif