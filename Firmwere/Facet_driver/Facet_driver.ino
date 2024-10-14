#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <U8g2lib.h>
#include <EEPROM.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define led 2
#define forward_btn 7
#define back_btn 13
#define select_btn 15
#define autoMode_toggle 14


//bool select_btn_state = 1;//1 becouse pinmode set to INPUT PULLUP
bool back_btn_state = 1;
bool fwd_btn_state = 1;
bool select_btn_prsd = 0; 
bool forward_btn_prsd = 0;
bool back_btn_prsd = 0;

long push_time = 0;
long hold_time = 0;
long releae_time = 0;

const int Facet_Val[11]={8,16,32,64,128, 12,24,48,72,96, 8};

bool autoMode = 1; //directly atteched to input pullup... 1=OFF,0=ON

//------TMCStepper-----------------------
//----Motor-1
#define EN_PIN           20 // Enable
#define DIR_PIN          18 // Direction
#define STEP_PIN         19 // Step
//----Motor-2
#define EN_PIN_2           10 // Enable
#define DIR_PIN_2          12 // Direction
#define STEP_PIN_2         11 // Step
// #define SW_RX            1 // TMC2208/TMC2224 SoftwareSerial receive pin
// #define SW_TX            0 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define VREF            26 // analog0 pin 26 for vRef Masurement

#define R_SENSE 0.11f // Match to your driver // SilentStepStick series use 0.11
#define RMS_I 950 // Match to your driver // SilentStepStick series use 0.11
#define DRIVER_ADDRESS 0x00 // TMC2209 driver address when MS1 LOW and MS2 LOW
#define STALL_VALUE     100 //0-255

TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
constexpr uint32_t steps_per_rev = 1600; //1.8` @ 200SperRev x 32us x 48:1 gear
int backlashCompSteps = 500;
//long StepsPerRev = 200;
bool shaft = false;

//------AccelStepper----------------------
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);
AccelStepper stepper2 = AccelStepper(stepper.DRIVER, STEP_PIN_2, DIR_PIN_2);

bool Stepper_Div = 0;
long currunt_Position;
//int stepsToMove;
int currunt_facet; //currunt running facet
int Facet_to_Cut; //currunt selected numbers of facets to cut

int ep_facet_num; //int to hold Currunt number of facet to EEprom
int ep_facet_val; //int to hold Currunt facet value to EEprom

int tmcVrefOut1(){int aVref = map(analogRead(VREF), 0, 800, 1, 250); return aVref * 10;}
int tmcVrefOut2(){int aVref = map(analogRead(VREF), 0, 800, 1, 250); return aVref * 10;}

int myFunction(float, float);
// put function definitions here:
int myFunction(float StepsPerRev, float FacetToCut) {
  return StepsPerRev / FacetToCut;
} int stepsperfacet;

const unsigned char epd_bitmap_Mcreeii [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x03, 0x03, 0xc0, 0x00, 0x00, 
	0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x03, 0x03, 0xc0, 0x01, 0x00, 0xf8, 0xff, 0x01, 0x00, 
	0x00, 0x00, 0x8e, 0x03, 0x07, 0xe0, 0x01, 0x00, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xe0, 0x01, 0x00, 0x18, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0xc1, 0x7f, 
	0x18, 0x80, 0xc1, 0x7f, 0xf0, 0x1f, 0x8e, 0x03, 0x1f, 0xf8, 0xe1, 0x7f, 0x18, 0x80, 0xe1, 0xff, 
	0xf8, 0x3f, 0x8e, 0x03, 0x1f, 0xf8, 0xf1, 0x7f, 0x18, 0xc0, 0x71, 0xe0, 0x1c, 0x38, 0x8e, 0x03, 
	0x3b, 0xdc, 0x71, 0x00, 0x18, 0xe0, 0x71, 0xc0, 0x1c, 0x30, 0x8e, 0x03, 0x33, 0xdc, 0x71, 0x00, 
	0xd8, 0xff, 0x70, 0xe0, 0x1c, 0x38, 0x8e, 0x03, 0x73, 0xce, 0x71, 0x00, 0xd8, 0x7f, 0x70, 0xff, 
	0xdc, 0x3f, 0x8e, 0x03, 0xe3, 0xce, 0x71, 0x00, 0x18, 0x1e, 0x70, 0x7f, 0xdc, 0x1f, 0x8e, 0x03, 
	0xe3, 0xc7, 0x71, 0x00, 0x18, 0x3c, 0x70, 0x00, 0x1c, 0x00, 0x8e, 0x03, 0xc3, 0xc3, 0xf1, 0x7f, 
	0x18, 0x78, 0xf0, 0x1f, 0xfc, 0x07, 0x8e, 0x03, 0xc3, 0xc3, 0xe1, 0x7f, 0x18, 0xf0, 0xe0, 0x1f, 
	0xf8, 0x07, 0x8e, 0x03, 0x83, 0xc1, 0xc1, 0x7f, 0x18, 0xe0, 0xc3, 0x1f, 0xf0, 0x07, 0x8e, 0x03
};

void u8g_Print()
{
  int currunt1 = tmcVrefOut1();
  int currunt2 = tmcVrefOut2();
  u8g2.clearBuffer();
  //u8g2.drawXBM(0, 47, 90, 16, epd_bitmap_Mcreeii);
  //u8g2_font_logisoso54_tn
  u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
  u8g2.drawStr(5, 61,"McReeii");
  u8g2.setFont(u8g2_font_logisoso42_tn);
  u8g2.setCursor(0,42);
  u8g2.print(currunt_facet);

  u8g2.setFont(u8g2_font_press_mel_tn);
  u8g2.setCursor(92,15);
  u8g2.print(Facet_to_Cut);
 
  u8g2.setFont(u8g2_font_5x8_tr);
  if (autoMode == 0){u8g2.drawStr(98,29,"AUTO");} else {u8g2.drawStr(96,29,"MANUL");}

  u8g2.setFont(u8g2_font_press_mel_tn);
  u8g2.setCursor(92,45);
  u8g2.print(ep_facet_num);

  u8g2.setFont(u8g2_font_6x12_mn);
  u8g2.setCursor(72,60);
  u8g2.print(currunt1);
  u8g2.setCursor(103,60);
  u8g2.print(currunt2);

  u8g2.drawLine(86, 0, 128, 0);//Horizontal Top of page
  u8g2.drawLine(0, 49, 128, 49);//Horizontal middle of page
  u8g2.drawLine(0, 63, 128, 63);//Horizontal last of page
  u8g2.drawLine(86, 20, 128, 20);//Horizontal next to Facet to Cut
  u8g2.drawLine(86, 31, 128, 31);//Horizontal next to Auto Button
  //u8g2.drawLine(116, 20, 116, 32);//Vertical next to Auto Button
  u8g2.drawLine(0, 49, 0, 64);//Vertical Start of the page
  u8g2.drawLine(86, 0, 86, 48); //Vertical middle of the page
  u8g2.drawLine(64, 49, 64, 64);//Vertical next to Vref1
  u8g2.drawLine(96, 49, 96, 64);//Vertical next to Vref2
  u8g2.drawLine(127, 0, 127, 64);//Vertical Last of page
  u8g2.sendBuffer();	
}

int getDelayTime(int, int);
int getDelayTime(int start_time, int end_time) {
  int btime = end_time - start_time ;
  return btime;
} int DelayTime;

long get_time(){
  long RPtime = millis();
  return RPtime;
}

void led_blink(int mS){
  digitalWrite(led, HIGH);
  delay(mS);
  digitalWrite(led, LOW);
}

void writeIntIntoEEPROM(int address, int number){
  EEPROM.write(address, number);
  if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
}
byte EEPval;
int readIntFromEEPROM(int address){
  EEPval = EEPROM.read(address);
  return EEPval;
}

void backlash(){
  Stepper_Div = 1;
  stepper2.setPinsInverted(Stepper_Div, false, true);
  stepper2.move(backlashCompSteps);
  while (stepper2.distanceToGo() != 0) {
    stepper2.runToPosition();
  }
  delay(10);
  motorMove();
  delay(10);

  Stepper_Div = 0;
  stepper2.setPinsInverted(Stepper_Div, false, true);
  stepper2.move(backlashCompSteps);
  while (stepper2.distanceToGo() != 0) {
    stepper2.runToPosition();
  }delay(10);
  
}

void change_Facet(){
  if ((digitalRead(select_btn) == LOW) && (select_btn_prsd == 0)) {
    select_btn_prsd = 1;
    push_time = get_time();
    led_blink(10);
    Serial.print("\nPsd select btn");
  }

  if ((digitalRead(select_btn) == HIGH) && (select_btn_prsd == 1)){
    select_btn_prsd = 0;
    releae_time = get_time();
    led_blink(10);
    DelayTime = getDelayTime(push_time,releae_time);
    //Serial.print( " //time_mS/10 - ");
    //Serial.print(DelayTime);
  }
  hold_time = get_time()-push_time;

  int currunt_sel_facet = ep_facet_val;

  if ((select_btn_prsd == 1) && (hold_time >= 1000)){     
    select_btn_prsd = 0;
    Serial.print("\nHld Sel Btn");
    delay(60); led_blink(10);
    //DelayTime = getDelayTime(push_time,releae_time);
    //ADD KEY HOLD TRIGGER HERE
    ep_facet_val ++;
    if (ep_facet_val > 9){ep_facet_val = 0; delay(10);}
    Facet_to_Cut = Facet_Val[ep_facet_val];
    writeIntIntoEEPROM(222, ep_facet_val);
  }
}

void motorMove(){
  if (forward_btn_prsd == 1){
      digitalWrite(led,1);
      Stepper_Div = false;
      delay(10);
      currunt_facet ++;
      stepper.setPinsInverted(Stepper_Div, false, true);
      stepper.move(stepsperfacet);
      while (stepper.distanceToGo() != 0){  
        stepper.runToPosition();
      }
      digitalWrite(led,0); delay(10);
      curruntFacet();
      writeIntIntoEEPROM(111, currunt_facet);
      Serial.print("Currunt Facet:");
      Serial.println(currunt_facet);
    }
    forward_btn_prsd = 0;

  if (back_btn_prsd == 1){
      digitalWrite(led,1);
      Stepper_Div = true;
      stepper.setPinsInverted(Stepper_Div, false, true);
      stepper.move(stepsperfacet);
      delay(10);
      currunt_facet --;
      while (stepper.distanceToGo() != 0) {
        stepper.runToPosition();
      }
      
      digitalWrite(led,0); delay(10);
      curruntFacet();
      writeIntIntoEEPROM(111, currunt_facet);
      Serial.print("Currunt Facet:");
      Serial.println(currunt_facet);
    }
    back_btn_prsd = 0;
    delay(10);
}

void curruntFacet(){
  if (currunt_facet > Facet_to_Cut){
    currunt_facet = 1;
  }

  if(currunt_facet < 1) {
    currunt_facet = Facet_to_Cut;
  }
}

void buttonPress(){

fwd_btn_state = digitalRead(forward_btn);  
back_btn_state = digitalRead(back_btn);
autoMode = digitalRead(autoMode_toggle);
  if (fwd_btn_state == 0)
  {
      forward_btn_prsd = 1;
      digitalWrite(led,HIGH);
      Serial.println("Fwd button pressed");
      delay(10);
      backlash();
  } 
  else
  { digitalWrite(led, LOW); }  

  if (back_btn_state == 0)
  {
    back_btn_prsd = 1;
    digitalWrite(led,HIGH);
    Serial.println("Back button pressed");
    delay(10);
    backlash();
  }
  else
  { digitalWrite(led, LOW); }
}

void AutoRun(){
  if (autoMode == 0){
    forward_btn_prsd = 1;
    backlash();
    delay(1000);
  }
  
}

void FactoryDataReset(){
  Serial.println("Factory data reset perfomed");
  writeIntIntoEEPROM(111, 1); // currnt facet
  writeIntIntoEEPROM(222, 0); //facet to cut
  Facet_to_Cut = Facet_Val[0];

  u8g2.clearBuffer();	
  u8g2.setCursor(0,30);
  u8g2.print("Factory Data Reset"); 
  u8g2.sendBuffer();	

  delay(2000);
}

void setup() {
  Serial.begin(115200);
  u8g2.setColorIndex(1);  // set the color to white
  u8g2.setDrawColor(1);
  u8g2.begin();///*Select=*/ BUTTON_SELECT_PIN, /*Right/Next=*/ BUTTON_FWD_PIN, /*Left/Prev=*/ BUTTON_PRV_PIN, /*Up=*/ BUTTON_UP_PIN, /*Down=*/ BUTTON_DOWN_PIN, /*Home/Cancel=*/ BUTTON_Exit_PIN);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_6x12_tr);
  delay(100);

  pinMode(forward_btn, INPUT_PULLUP);
  pinMode(back_btn, INPUT_PULLUP);
  pinMode(select_btn, INPUT_PULLUP);
  pinMode(autoMode_toggle, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // Disable driver in hardware
  pinMode(EN_PIN_2, OUTPUT);
  digitalWrite(EN_PIN_2, HIGH); // Disable driver in hardware

  driver.beginSerial(115200);
  driver.rms_current(RMS_I);   
  driver.pwm_autoscale(1);
  driver.pwm_autograd(1);
  driver.microsteps(16);
  // driver.semin(5);
  // driver.semax(2);
  // driver.sedn(0b01);
  // driver.shaft(shaftVal);
  // // TCOOLTHRS needs to be set for stallgaurd to work //
  // driver.TCOOLTHRS(0xFFFFF); // 20bit max
  // driver.SGTHRS(STALL_VALUE);
  // attachInterrupt(digitalPinToInterrupt(STALL_PIN_X), stallInterruptX, RISING);
  delay(100);

  stepper.setMaxSpeed(3000); // 3.6rev/s @ 6400steps/rev
  stepper.setAcceleration(5000); // 2000mm/s^2
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.setCurrentPosition(0);
  stepper.enableOutputs();

  stepper2.setMaxSpeed(1000); // 3.6rev/s @ 6400steps/rev
  stepper2.setAcceleration(2000); // 2000mm/s^2
  stepper2.setEnablePin(EN_PIN_2);
  stepper2.setPinsInverted(false, false, true);
  stepper2.setCurrentPosition(0);
  stepper2.enableOutputs();


  EEPROM.begin(512);
  delay(100);

  if(digitalRead(15) == LOW && digitalRead(7) == LOW){FactoryDataReset();}
  ep_facet_num = readIntFromEEPROM(111);//read address 111 (for currunt running facet)
  ep_facet_val = readIntFromEEPROM(222);//for currunt numbers of facets to cut
  currunt_facet = ep_facet_num;
  Facet_to_Cut = Facet_Val[ep_facet_val];

  stepsperfacet = myFunction(steps_per_rev, Facet_to_Cut); 

  driver.VACTUAL(0);
  delay(1000);
  digitalWrite(EN_PIN, LOW); // Enable driver in hardware
  digitalWrite(EN_PIN_2, LOW); // Enable driver in hardware
}

void loop() {
  stepsperfacet = myFunction(steps_per_rev, Facet_to_Cut);
  AutoRun();
  buttonPress();
  u8g_Print();
  change_Facet();
  delay(50);
 
}

