#include "Button.h"
#include "Pins.h"

const int Buttton[6]= {BUTTON_SELECT_PIN, BUTTON_Exit_PIN, BUTTON_PRV_PIN, BUTTON_DOWN_PIN, BUTTON_FWD_PIN, BUTTON_UP_PIN };
char BtnName[6]={'S','E','P','D','F','U'};
bool pressed[6]={0,0,0,0,0,0};

bool Buttton_clicked = 0;
int Pressed_btn = 0;
char Pressed_btn_name = 'O';
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
  long esptime = millis();
  return esptime;
}

void led_blink(int mS){
  digitalWrite(rpled, HIGH);
  delay(mS);
  digitalWrite(rpled, LOW);
}

void unclick(){
  Buttton_clicked = 0;
  Pressed_btn = 0;
  Pressed_btn_name = 'O';
  Long_press = 0;
  Trrigger = 0;
}

void click(){
  //-----i2c_esp32----click-----
  if (btn_click_i2c == true){
    Pressed_btn = click_out;
    Pressed_btn_name = click_name_out;
    if (click_type_out == 2){Long_press = 1;} else {Long_press = 0;}
    Trrigger = btn_click_i2c;

    Serial.print(Pressed_btn); Serial.print(" >> ");
    Serial.print(Pressed_btn_name); Serial.print(" >> ");
    Serial.print(Long_press); Serial.print(" >> ");
    Serial.println(Trrigger);
    btn_click_i2c = false;
  }

  //-----RP2040----click-----
  for (int btn = 0; btn < 6; btn++)
  {
    if ((digitalRead(Buttton[btn]) == LOW) && (pressed[btn] == 0)) {
      Buttton_clicked = 1;
      pressed[btn] = 1;
      clickType = CLICK;
      push_time = get_time();
      led_blink(10);
      Pressed_btn = btn + 10; //Pass value to int whitch Button is pressed
      Pressed_btn_name = BtnName[btn]; //Pass value to int what Button is pressed
      Serial.print("\nPsd ");
      Serial.print(Pressed_btn);
      Serial.print(Pressed_btn_name);
      Serial.print(" - ");
      Trrigger = true;
    }

    if ((digitalRead(Buttton[btn]) == HIGH) && (pressed[btn] == 1)){
      Buttton_clicked = 0;
      pressed[btn] = 0;
      releae_time = get_time();
      led_blink(10);
      DelayTime = getDelayTime(push_time,releae_time);
      Serial.print( " //time_mS/10 - ");
      Serial.print(DelayTime);
    }

    hold_time = get_time()-push_time;

    if ((Buttton_clicked == 1) && (hold_time >= 1000)){     
      Serial.print("\nHld ");
      Serial.print(Pressed_btn);
      Serial.print(Pressed_btn_name);
      Serial.print(" - ");
      Serial.print(hold_time);
      clickType = HOLD;
      delay(60); led_blink(10);
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

bool btn_LongPress_Out() {
  bool btn_Val = Long_press;
  return btn_Val;
}

uint8_t btn_Click_Out() {
  uint8_t btn_Val = Pressed_btn;
  return btn_Val;
}

char btn_Name_Click_Out() {
  char btn_Val = Pressed_btn_name;
  return btn_Val;
}

uint8_t btn_Click_Type_Out() {
  uint8_t clk_type = clickType;
  return clk_type;
}

int btn_Hold_ms_Out() {
  int btn_Val = DelayTime;
  return btn_Val;
}

void Button_init(){
  pinMode(rpled, OUTPUT);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  pinMode(BUTTON_FWD_PIN, INPUT_PULLUP); // next button
  pinMode(BUTTON_PRV_PIN, INPUT_PULLUP); // prv button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button
  pinMode(BUTTON_Exit_PIN, INPUT_PULLUP);
  delay(100);
}

void Button_func(){
  Trrigger = false;
  Long_press = false;
  clickType = NONE;
  click();
}
