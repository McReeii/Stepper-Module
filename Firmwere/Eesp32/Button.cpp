#include "Button.h"
#include "Pins.h"

const int EspNowCh[4]={13,34,25,26};
const int Buttton[6]= {32,  33,   14,   27,   12,   4 };
char BtnName[6]={'S', 'E',  'P',  'D',  'F',  'U'};
bool pressed[6]={0,0,0,0,0,0};

bool Buttton_clicked = 0;
int Pressed_btn = 0;
char Pressed_btn_name ;
bool Long_press;
bool Trrigger = 0;

long push_time = 0;
long hold_time = 0;
long releae_time = 0;

enum ClickType {NONE ,CLICK, HOLD};
ClickType clickType = NONE;

int getDelayTime(int, int);
int getDelayTime(int start_time, int end_time) {
  int btime = end_time - start_time ;
  return btime;
}
int DelayTime;

long get_time(){
  long esptime = esp_timer_get_time()/1000;
  return esptime;
}

void led_blink(int mS){
  digitalWrite(ESP_led, HIGH);
  delay(mS);
  digitalWrite(ESP_led, LOW);
}

void click(){
  for (int btn = 0; btn < 6; btn++)
  {
    if ((digitalRead(Buttton[btn]) == LOW) && (pressed[btn] == 0)) {
      Buttton_clicked = 1;
      pressed[btn] = 1;
      push_time = get_time();
      clickType = CLICK;
      led_blink(10);
      Pressed_btn = btn + 10; //Pass value to int whitch Button is pressed
      Pressed_btn_name = BtnName[btn]; //Pass value to int what Button is pressed
      // Serial.print("\nPsd ");
      // Serial.print(Pressed_btn);
      // Serial.print(Pressed_btn_name);
      // Serial.print(" - ");
      Trrigger = true;
    }

    if ((digitalRead(Buttton[btn]) == HIGH) && (pressed[btn] == 1)){
      Buttton_clicked = 0;
      pressed[btn] = 0;
      releae_time = get_time();
      led_blink(10);
      DelayTime = getDelayTime(push_time,releae_time);
      // Serial.print( " //time_mS/10 - ");
      //Serial.println(DelayTime);
    }

    hold_time = get_time()-push_time;

    if ((Buttton_clicked == 1) && (hold_time >= 1000)){
      // Serial.print("\nHld ");
      // Serial.print(Pressed_btn);
      // Serial.print(Pressed_btn_name);
      // Serial.print(" - ");
      // Serial.print(hold_time);    
      clickType = HOLD;
      delay(20); led_blink(10);
      //DelayTime = getDelayTime(push_time,releae_time);
      Trrigger = true; Long_press = true;
      //ADD KEY HOLD TRIGGER HERE
    }
  }
}
//----------------------------------------------------------------------------------------------------------

bool btn_Trigger_Out() {
  bool btn_Val = Trrigger;
  return btn_Val;
}

uint8_t btn_LongPress_Out() {
  uint8_t btn_Val = Long_press;
  return btn_Val;
}

uint8_t btn_Click_Out() {
  uint8_t btn_Val = Pressed_btn;
  return btn_Val;
}

uint8_t btn_Name_Click_Out() {
  uint8_t btn_Val = Pressed_btn_name;
  return btn_Val;
}

uint8_t btn_Click_Type_Out() {
  uint8_t clk_type = clickType;
  return clk_type;
}

uint16_t btn_Hold_ms_Out() {
  uint16_t btn_Val = DelayTime;
  return btn_Val;
}

void Button_init(){
  pinMode(ESP_led, OUTPUT);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // Up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // Select button
  pinMode(BUTTON_FWD_PIN, INPUT_PULLUP); // Next button
  pinMode(BUTTON_PRV_PIN, INPUT_PULLUP); // Prv button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // Down button
  pinMode(BUTTON_Exit_PIN, INPUT_PULLUP); // Exit button
}

void Button_func(){

  Trrigger = false;
  Long_press = false;
  clickType = NONE;
  click();

}