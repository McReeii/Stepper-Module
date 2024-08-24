#include "Arduino.h"
#include "Button.h"
#include "Pins.h"

const int EspNowCh[4]={15, 13, 12, 14};
const int Buttton[3]= {4, 5, 14};
char BtnName[3]={'P', 'F', 'S'};
bool pressed[3]={0,0,0};

bool Buttton_clicked = 0;
int Pressed_btn = 0;
char Pressed_btn_name ;

long push_time = 0;
long hold_time = 0;
long releae_time = 0;

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
  digitalWrite(ESP8266_led, HIGH);
  delay(mS);
  digitalWrite(ESP8266_led, LOW);
}

void click(){

  for (int btn = 0; btn < 3; btn++)
  {
    if ((digitalRead(Buttton[btn]) == LOW) && (pressed[btn] == 0)) {
      Buttton_clicked = 1;
      pressed[btn] = 1;
      push_time = get_time();
      led_blink(10);
    }

    if ((digitalRead(Buttton[btn]) == HIGH) && (pressed[btn] == 1)){
      Buttton_clicked = 0;
      pressed[btn] = 0;
      Pressed_btn = btn; //Pass value to int whitch Button is pressed
      Pressed_btn_name = BtnName[btn]; //Pass value to int what Button is pressed
      Serial.print(Pressed_btn);
      Serial.println(Pressed_btn_name);
      
      releae_time = get_time();
      led_blink(10);
      DelayTime = getDelayTime(push_time,releae_time);
      //Serial.print( " //time_mS/10 - ");
      //Serial.println(DelayTime);
    }

    hold_time = get_time()-push_time;
    //ADD KEY CLICK TRIGGER HERE-----
    //
    if ((Buttton_clicked == 1) && (hold_time >= 1000)){      
      Serial.println(hold_time);
      led_blink(10);
      delay(90);

      //ADD KEY HOLD TRIGGER HERE-----
    }

  }
}

void pin_mode(){
     pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);                                           
     pinMode(BUTTON_FWD_PIN, INPUT_PULLUP);                                           
     pinMode(BUTTON_PRV_PIN, INPUT_PULLUP);                                           
     pinMode(ESP8266_led, OUTPUT);  
}

//----------------------------------------------------------------------------------------------------------

  void Button_init(){
    pin_mode();
  }

  void Button_Func(){
    click();
    delay(10);
  }
