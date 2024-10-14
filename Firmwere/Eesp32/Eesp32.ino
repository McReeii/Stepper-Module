#include "Button.h"
#include "Comm.h"
#include "EspNow32.h"

char i2c_btn_cmdtype = 'B';

uint8_t click_out;
uint8_t click_name_out;
uint8_t click_type_out;
uint16_t hold_ms_out;

void setup(){
  EspSerial();
  Button_init();
  EspNow32_init();
  //EspNowSend();
} 

void loop(){
  Button_func();

  click_out = btn_Click_Out();
  click_name_out = btn_Name_Click_Out();
  click_type_out= btn_Click_Type_Out();
  hold_ms_out= btn_Hold_ms_Out();

  bool New_Msg = New_Msg_Rcv();
  if (New_Msg == 1) {
    delay(20);
    char btn_Val = read_btn_val();
    char btn_type = read_clk_typ();
    Serial.print(btn_Val);
    Serial.print(btn_type);
    Serial.println("New MESSAGE");
    if (btn_Val == 'F' || btn_Val == 'P' || btn_Val == 'S'){
      WireSend(hold_ms_out, btn_Val, btn_type, click_out, 0x55, i2c_btn_cmdtype);
      }
    New_Msg = false; // Set Ping to FALSE to Stop if loop.
  }

  bool trigger = btn_Trigger_Out();
  if (trigger == true){
    Serial.print(click_out); Serial.print(" >> ");
    Serial.print(click_name_out); Serial.print(" >> ");
    Serial.print(click_type_out); Serial.print(" >> ");
    Serial.println(hold_ms_out);
    WireSend(hold_ms_out, click_name_out, click_type_out, click_out, 0x55, i2c_btn_cmdtype);
    //Serial.print(read_btn_val());
    //Serial.print(read_clk_typ());
    EspNowSend();
  }
}