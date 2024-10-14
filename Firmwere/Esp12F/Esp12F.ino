#include "Button.h"
#include "Comm.h"
#include "EspNow8266.h"
//#include <esp_

void setup(){
EspSerial();
delay(500);
Button_init();
EspNow8266_init();
EspNowSend(); // send Pairing MSG

}

void loop(){
Button_func();
bool EspPaired = EspNowAutoPair();

if (EspPaired == 0) {delay(1000); led_blink(10);}

bool trigger = btn_Trigger_Out();
  if (trigger == true){
    Serial.print(btn_Click_Out());
    Serial.print(btn_Name_Click_Out());
    Serial.print(btn_Click_Type_Out());
    Serial.println(btn_Hold_ms_Out());

    getReadings(btn_Name_Click_Out(), btn_Click_Type_Out());
    EspNowSend();
  }
}
