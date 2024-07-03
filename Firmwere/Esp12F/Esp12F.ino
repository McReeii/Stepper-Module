const int EspNowCh[4]={15, 13, 12, 14};
const int Buttton[6]= {16, 4, 5};
char BtnName[6]=      {'S', 'P', 'F',};
bool pressed[6]={0,0,0,0,0,0};

bool Buttton_clicked = 0;
int Pressed_btn = 0;
char Pressed_btn_name ;

int temp;
long push_time = 0;
long hold_time = 0;
long releae_time = 0;

#define ESP8266_led       2  // pin for LED_light
#define BUTTON_SELECT_PIN 16  // pin for Select Button
#define BUTTON_FWD_PIN    5  // pin for NEXT button
#define BUTTON_PRV_PIN    4  // pin for BACK button 


int getDelayTime(int, int);
int getDelayTime(int start_time, int end_time) {
  int btime = end_time - start_time ;
  return btime;
}
int DelayTime;


long get_time(){
  long esptime = esp_timer_get_time()/10000;
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
      Serial.print(Pressed_btn_name);
      
      releae_time = get_time();
      led_blink(10);
      DelayTime = getDelayTime(push_time,releae_time);
      Serial.print( " //time_mS/10 - ");
      Serial.println(DelayTime);
    }

    hold_time = get_time()-push_time;
    //
    if ((Buttton_clicked == 1) && (hold_time >= 100)){      
      Serial.println(hold_time);
      led_blink(10);
      delay(90);
    }

  }
}
//----------------------------------------------------------------------------------------------------------





void setup(){

  Serial.begin(115200); //ESP_DEFAULT_UART
  delay(100);

  while (!Serial){
    yield();  delay(100);
  }
  
  pinMode(ESP8266_led, OUTPUT);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // Select button
  pinMode(BUTTON_FWD_PIN, INPUT_PULLUP); // Next button
  pinMode(BUTTON_PRV_PIN, INPUT_PULLUP); // Prv button

}


void loop()
{
click();

delay(10);

}
