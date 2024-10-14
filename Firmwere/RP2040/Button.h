#ifndef Button_h
#define Button_h

#include "Arduino.h"

extern bool btn_click_i2c;
extern uint8_t click_out;
extern char click_name_out;
extern uint8_t click_type_out;
extern uint16_t hold_ms_out;
extern char i2c_btn_cmdtype;

#define DBLCLK_DELAY 100

void click();
void unclick();
void Button_init();
void Button_func();
bool btn_Trigger_Out();
bool btn_LongPress_Out();
uint8_t btn_Click_Out();//value starts from 10 insted of 0 to 15
char btn_Name_Click_Out();
uint8_t btn_Click_Type_Out();
int btn_Hold_ms_Out();

#endif

//PRE DEFINED INTINERS FOR COPY & PASTE
    // int btn_Click = btn_Click_Out();
    // char btn_Name_Click = btn_Name_Click_Out();
    // int btn_Hold = btn_Hold_Out();
    // int btn_Hold_ms = btn_Hold_ms_Out();