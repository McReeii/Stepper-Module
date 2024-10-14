#include "SerialUSB.h"
#include "TMC2209.h"

#include <TMCStepper.h>

TMC2209Stepper driver(&TMC_Serial, R_SENSE, DRIVER_ADDRESS);


bool TMC_dir = false; //direction for TMC SHAFT 
bool TMC_enable = false; // to start-stop motor 0=ON, 1=OFF
int CurruntSpeed;

bool HomeNeeded = 1;
int Hall_sens;

char btn_pressed;
char fwd_b = 'F';
char prv_b = 'P';
char sel_b = 'S';

int SpeedCtrl = 0; // speed selected by the user
int timeDelay = 1; //can not be 0 ...// Strocking delay selected by user in strocking function
bool MotorStop = false;

void UserInput(){
  btn_pressed = btn_Name_Click_Out();
  if(currentscreen() == 1){
    switch(btn_pressed){
      case 'F':
        timeDelay = timeDelay+1; sleep_ms(50); if (timeDelay > MaxTimeDelay){timeDelay = MaxTimeDelay;}
        unclick();
      break;

      case 'U':
        SpeedCtrl = SpeedCtrl+1; sleep_ms(250); if (SpeedCtrl > speedLimit){SpeedCtrl = speedLimit;}
        unclick();
      break;

      case 'P':
        timeDelay = timeDelay-1; sleep_ms(50); if (timeDelay <= 1){timeDelay = 1;}
        unclick();
      break;    
      
      case 'D':
        SpeedCtrl = SpeedCtrl-1; sleep_ms(250); if (SpeedCtrl < 0){SpeedCtrl = 0;}
        unclick();
      break;

      case 'S':
        MotorStop = !MotorStop;
        unclick();
      break;
      default: 
      break;
    } // SWITCH
  } // IF
}

void Tmc2209_init(){
  TMC_Serial.begin(115200); //TMC hardwere Communication
  delay(100);

  pinMode(HALL_PIN, INPUT);
  pinMode(PIN_DIAG, INPUT);
  pinMode(PIN_INDEX, INPUT);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);

  driver.begin();
  driver.toff(5);
  driver.blank_time(24);
  driver.rms_current(950); // in mA
  driver.microsteps(MICROSTEPS);
  driver.TCOOLTHRS(0xFFFFF); // 20bit max
  driver.pwm_autoscale(true);
  driver.pwm_autograd(true);
  driver.semin(5);
  driver.semax(2);
  driver.sedn(0b01);
  driver.SGTHRS(STALL_VALUE); // Stallguard < threshold -> DIAG pin goes up

  CurruntSpeed = 0;
  driver.VACTUAL(0);
  delay(500);
}

//--------------DATA OUTPUT------------------
int tmcSpeedOut(int TMC_Speed){int TmcSpeed = TMC_Speed; return TmcSpeed;}
int tmcDirOut(){return TMC_dir;}
int tmcVrefOut(){int aVref = map(analogRead(VREF), 0, 800, 1, 250); return aVref * 10;}
int Speed_UserInput_Out(){return SpeedCtrl;}
int timeDelay_UserInput_Out(){return timeDelay;}
bool HallOut(){bool HallPin = (digitalRead(HALL_PIN)); return HallPin;}

//--------------DATA INPUT------------------

void TmcSpeed(int Vactual){
  if (CurruntSpeed != Vactual){
    Serial.print("Tmc Speed:");
    Serial.println(CurruntSpeed);
    digitalWrite(PIN_EN, 0);
    if (CurruntSpeed < Vactual){ // If Currunt speed is lower then Wanted ==> Speed Up
    for (long j = CurruntSpeed; j <= Vactual; j= j+50) {driver.VACTUAL(j);}} // Speed up to maxSpeed

    if (CurruntSpeed > Vactual){ // If Currunt speed is Higher then Wanted ==> Slow Down
    for (long j = CurruntSpeed; j >= Vactual; j= j-50) {driver.VACTUAL(j);}} // Speed up to maxSpeed

    driver.VACTUAL(Vactual);
  }
  CurruntSpeed = Vactual;
}

void TmcStop(){
  CurruntSpeed = 0;
  driver.VACTUAL(0);
  //TMC_enable = false;
  //digitalWrite(PIN_EN, TMC_enable);
  sleep_ms(50);
}

void PrintTmcData(){
  Serial.print("Continuous Mode, Dir:");
  Serial.print(TMC_dir);
  Serial.print(", Power:");
  Serial.print(!TMC_enable);
  Serial.print(", Speed:");
  Serial.println(CurruntSpeed);
}

//----------------------------------------------------------------------------------------------------------
//void continuous_Run(int TMC_Speed){
void continuous_Run(){
  int maxSpeed = speedMultiplyer*SpeedCtrl;
  //Serial.println(btn_pressed);//MotorStop);

  if(MotorStop == false){
    TmcSpeed(maxSpeed);
    digitalWrite(rpled,HIGH);}

  if(btn_pressed == prv_b) { // if forward btn is pressed
    for (long k = maxSpeed; k >= 0; k= k-50){ // Speed down to 0
      driver.VACTUAL(k);
      sleep_ms(2);
    }
      driver.VACTUAL(0);
      TMC_dir = 0;
      driver.shaft(TMC_dir);
    sleep_ms(50);
    for (long j = 0; j <= maxSpeed; j= j+50){ // Speed up to maxSpeed
      driver.VACTUAL(j);
      sleep_ms(2);
    }
    PrintTmcData();
    CurruntSpeed = maxSpeed;
  }

  if(btn_pressed == fwd_b) { // if Backward btn is pressed
    for (long k = maxSpeed; k >= 0; k= k-50){ // Speed down to 0
      driver.VACTUAL(k);
      sleep_ms(2);
    }
    driver.VACTUAL(0);
    TMC_dir = 1;
    driver.shaft(TMC_dir);
    sleep_ms(50);
    for (long j = 0; j <= maxSpeed; j= j+50){ // Speed up to maxSpeed
      driver.VACTUAL(j);
      sleep_ms(2);
    }
    PrintTmcData();
    CurruntSpeed = maxSpeed;
  } 
  digitalWrite(rpled, LOW);
  
  if(MotorStop == true){TmcSpeed(0);}
}

void Homming(){
  Hall_sens = digitalRead(HALL_PIN);
  if (HomeNeeded == 1){
    if (Hall_sens == 1){
      driver.shaft(TMC_dir);
      driver.VACTUAL(300);
    }
    else if (Hall_sens == 0){
        driver.shaft(TMC_dir);
        driver.VACTUAL(0);
    }
    HomeNeeded = 0;
  }
}


//----------------------------------------------------------------------------------------------------------
void Stroking(){
  int maxSpeed = speedMultiplyer*SpeedCtrl;
  digitalWrite(PIN_EN, LOW);
  if(MotorStop == false){
    digitalWrite(rpled,HIGH);
    TMC_dir = !TMC_dir;
    driver.shaft(TMC_dir);
    delay(10);
    digitalWrite(rpled,LOW);
    TmcSpeed(maxSpeed);
    sleep_ms(timeDelay*100);
    TmcSpeed(0); delay(50);
  }
  if(MotorStop == true){TmcSpeed(0);}
}


//void Stroking(int St_Speed , int timeGap){
// void Stroking(){
//   int maxSpeed = speedMultiplyer*SpeedCtrl;
//   digitalWrite(PIN_EN, LOW);

//   for (int i = 0; i <= 1; i++) {
//     digitalWrite(rpled,HIGH);
//     TMC_dir = !TMC_dir;
//     driver.shaft(TMC_dir);
//     driver.VACTUAL(0);
//     sleep_ms(10);
//     digitalWrite(rpled,LOW);
//     for (long j = 0; j <= maxSpeed; j= j+50){ // Speed up to maxSpeed
//       driver.VACTUAL(j);
//       sleep_ms(2);
//     }

//     sleep_ms(timeDelay*100);
//     for (long k = maxSpeed; k >= 0; k= k-50){ // Speed down to 0
//       driver.VACTUAL(k);
//       sleep_ms(2);
//     }
//   }
//   driver.VACTUAL(0);
// }

