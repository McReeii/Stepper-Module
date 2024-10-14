#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <U8g2lib.h>
#include <Wire.h>

//------U8G2_SH1106_Config------
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//------Stepper_PINS------
//#define PIN_INDEX          // Indexing
#define PIN_DIAG          25 // diagnostic
#define PIN_EN            20 // Enable (inverted) 
#define PIN_DIR           22 // direction
#define PIN_STEP          21 // Step
//#define PIN_RX          1 // RX
//#define PIN_TX          0 // TX
#define TMC_Serial Serial1  // Serial1
#define DRIVER_ADDRESS    0b00  // MS1 and MS2 = address
#define STALL_VALUE       100  // [0...255]

//------TMCStepper_Config------
#define R_SENSE         0.11f // Match to your driver // SilentStepStick series use 0.11
#define RMS_I           950 // Match to your driver // SilentStepStick series use 0.11
#define DRIVER_ADDRESS  0b00 // TMC2209 driver address when MS1 LOW and MS2 LOW
#define STALL_VALUE     100 //0-255
#define MICROSTEPS      16
#define VREF            26 // analog0 pin 26 for vRef Masurement

TMC2209Stepper driver(&TMC_Serial, R_SENSE, DRIVER_ADDRESS);
constexpr uint32_t StepsPerRev = 1800; //1.8` @ 200SperRev x 32us x 48:1 gear
bool TMC_dir = false; //direction for TMC SHAFT
bool TMC_enable = false; // to start stop motor

//------AccelStepper_Config------
AccelStepper stepper = AccelStepper(stepper.DRIVER, PIN_STEP, PIN_DIR);
bool ACL_Div = 0;// Direction for AccelStepper
long currunt_Position;

#define led               2
#define BUTTON_SELECT_PIN 15 // pin for Select Button
#define BUTTON_Exit_PIN   6 // pin for Exit button
#define BUTTON_FWD_PIN    13 // pin for NEXT button
#define BUTTON_DOWN_PIN   3 // pin for DOWN button
#define BUTTON_PRV_PIN    7 // pin for BACK button 
#define BUTTON_UP_PIN     14 // pin for UP button                                          

#define HALL_PIN          27 //Hall Sensor input (digital)
bool HomeNeeded = 1; 
//Input pin for the button, Button is active high, Disable internal pull-up resistor
//------Buttons_Config------

bool button_fwd_clicked = 0;
bool button_prv_clicked = 0;
bool button_exit_clicked = 0;
bool button_up_clicked = 0; // only perform action when button is clicked, and wait until another press
bool button_select_clicked = 0; // same as above
bool button_down_clicked = 0; // same as above

int item_selected = 0; // which item in the menu is selected
int item_sel_previous; // previous item - used in the menu screen to draw the item before the selected one
int item_sel_next; // next item - used in the menu screen to draw next item after the selected one
int EnterMenuItem;

int current_screen = 0;   // 0 = menu, 1 = screenshot, 2 = qr
int current_menu = 0;   // 0 = Home

//------Functions------
int calc360Image(float, float); //Steps per facet Counter
// put function definitions here:
int calc360Image(float StepsPerRev, float FacetToCut) {
  return StepsPerRev / FacetToCut;
} int stepsPerFacet;

int Facet_to_Cut;
int currunt_facet;

//---PreDefineValues----!!!!DO_NOT_CHANGE!!!!
//int PotVal = 0;
//int Pot = 0;
int SpeedCtrl = 0;
int timeDelay = 1; //can not be 0

//---Settings----
int speedMultiplyer = 200;
int speedLimit = 200;

// all the arrays below are generated from images using Image Magick
// scroll down to see the actual code

const unsigned char upir_logo [] PROGMEM = {  
  0x67, 0x73, 0x57, 0x17, 0x35, 0x03, 0x15, 0x13, 0x51, 0x65, 0x75, 0x57, };

//----------------------------------------------------------------------------------------------------------

const unsigned char bitmap_icon_fireworks [] PROGMEM = {
  0x00, 0x00, 0x00, 0x10, 0x00, 0x29, 0x08, 0x10, 0x08, 0x00, 0x36, 0x00, 
  0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x08, 
  0x20, 0x08, 0x50, 0x00, 0x20, 0x00, 0x00, 0x00, };

const unsigned char bitmap_icon_strocking [] PROGMEM = {
	0x00, 0x00, 0x60, 0x00, 0x30, 0x00, 0x18, 0x00, 0x0c, 0x00, 0x86, 0xff, 0xff, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0xff, 0xff, 0x61, 0x00, 0x30, 0x00, 0x18, 0x00, 0x0c, 0x00, 0x06, 0x00, 0x00 };

const unsigned char bitmap_icon_dashboard [] PROGMEM = {
  0xE0, 0x07, 0x18, 0x18, 0x84, 0x24, 0x0A, 0x40, 0x12, 0x50, 0x21, 0x80, 
  0xC1, 0x81, 0x45, 0xA2, 0x41, 0x82, 0x81, 0x81, 0x05, 0xA0, 0x02, 0x40, 
  0xD2, 0x4B, 0xC4, 0x23, 0x18, 0x18, 0xE0, 0x07, };

const unsigned char bitmap_icon_3dcube [] PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0xE0, 0x06, 0x98, 0x18, 0x86, 0x60, 0x8A, 0x50, 
  0xA2, 0x45, 0x82, 0x40, 0xA2, 0x44, 0x82, 0x40, 0xA2, 0x45, 0x8A, 0x50, 
  0x86, 0x60, 0x98, 0x18, 0xE0, 0x06, 0x80, 0x01, };  


const unsigned char bitmap_icon_settings [] PROGMEM = {
	0x80, 0x01, 0x40, 0x02, 0x58, 0x1a, 0x64, 0x26, 0x04, 0x20, 0x08, 0x10, 0x8e, 0x71, 0x41, 0x82, 
	0x41, 0x82, 0x8e, 0x71, 0x08, 0x10, 0x04, 0x20, 0x64, 0x26, 0x58, 0x1a, 0x40, 0x02, 0x80, 0x01,};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 384)
const unsigned char* bitmap_icons[8] = {
  
  bitmap_icon_fireworks,
  bitmap_icon_strocking,
  bitmap_icon_dashboard,
  bitmap_icon_3dcube,
  bitmap_icon_settings,
};


// 'scrollbar_background', 8x64px
const unsigned char bitmap_scrollbar_background [] PROGMEM = {
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x00, };


// 'item_sel_outline', 128x21px
const unsigned char bitmap_item_sel_outline [] PROGMEM = {
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0xF8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 
  };


// ------------------ end generated bitmaps from image2cpp ---------------------------------

//----------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); // Serial Port communication Via USB for serial print
  TMC_Serial.begin(115200); //TMC hardwere Communication
 
  delay(1000);
  Serial.println("USB Serial: Connected");
  Serial.println("TMC Driver: Connected");
 
  pinMode(HALL_PIN, INPUT);
  pinMode(led, OUTPUT);
  pinMode(PIN_DIAG, INPUT);
  //pinMode(PIN_INDEX, INPUT);
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

  stepper.setMaxSpeed(1000); // 3.6rev/s @ 6400steps/rev
  stepper.setAcceleration(2000); // 2000mm/s^2
  stepper.setEnablePin(PIN_EN);
  stepper.setPinsInverted(false, false, true);
  stepper.setCurrentPosition(0);
  stepper.enableOutputs();

  u8g2.setColorIndex(1);  // set the color to white
  u8g2.begin(); // not needed in this lib (/*Select=*/ BUTTON_SELECT_PIN, /*Right/Next=*/ BUTTON_NEXT_PIN, /*Left/Prev=*/ BUTTON_PRV_PIN, /*Up=*/ BUTTON_UP_PIN, /*Down=*/ BUTTON_DOWN_PIN, /*Home/Cancel=*/ BUTTON_Exit_PIN);
  u8g2.setBitmapMode(1);

  // define pins for buttons
  // INPUT_PULLUP means the button is HIGH when not pressed, and LOW when pressed
  // since it´s connected between some pin and GND
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  pinMode(BUTTON_FWD_PIN, INPUT_PULLUP); // next button
  pinMode(BUTTON_PRV_PIN, INPUT_PULLUP); // prv button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button
  pinMode(BUTTON_Exit_PIN, INPUT_PULLUP);

}
//----------------------------------------------------------------------------------------------------------
void setup1() {

}
//----------------------------------------------------------------------------------------------------------
void continuous_Run_u8g(int TMC_Speed){

  u8g2.clearBuffer();	
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0,15);
  u8g2.print("Dir: ");
  u8g2.setCursor(40,15);
  u8g2.print(TMC_dir);
  u8g2.setCursor(0,30);
  u8g2.print("Speed");
  u8g2.setCursor(40,30);
  u8g2.print(TMC_Speed);
  u8g2.setCursor(0,45);
  u8g2.print("Press L/R for direction");
  u8g2.sendBuffer();
  currentscreen();

}
//----------------------------------------------------------------------------------------------------------
void Settings_menu()
{
  int aVref = map(analogRead(VREF), 0, 800, 1, 250);

  u8g2.clearBuffer();	
  u8g2.setFont(u8g2_font_5x8_mr);

  u8g2.setCursor(0,8);
  u8g2.print("MOTOR CURRENT: ");
  u8g2.setCursor(80,8);
  u8g2.print(aVref*10);
  u8g2.setCursor(110,8);
  u8g2.print("mA");

  u8g2.setCursor(0,16);
  u8g2.print("SPEED_X: ");
  u8g2.setCursor(80,16);
  u8g2.print(speedMultiplyer);
  u8g2.setCursor(110,16);
  u8g2.print("Pt");

  u8g2.setCursor(0,24);
  u8g2.print("SPEED_LIMIT: ");
  u8g2.setCursor(80,24);
  u8g2.print(speedLimit);
  u8g2.setCursor(110,24);
  u8g2.print("N");

  u8g2.setCursor(0,32);
  u8g2.print("HALL: ");
  u8g2.setCursor(80,32);
  u8g2.print(digitalRead(HALL_PIN));
  u8g2.setCursor(110,32);
  u8g2.print("0");

  u8g2.sendBuffer();
  currentscreen();
  sleep_ms(120);
  
}
//----------------------------------------------------------------------------------------------------------
void Stroking_u8g(int TMC_Speed, int timeGap){

  u8g2.clearBuffer();	
  u8g2.setFont(u8g2_font_ncenB08_tr);	
  u8g2.setCursor(0,15);
  u8g2.print("Strock: ");
  u8g2.setCursor(40,15);
  u8g2.print(timeGap);
  u8g2.setCursor(0,30);
  u8g2.print("Speed");
  u8g2.setCursor(40,30);
  u8g2.print(TMC_Speed);
  u8g2.setCursor(0,45);
  u8g2.print("Press L/R for direction");
  u8g2.sendBuffer();
  currentscreen();
}

void image360_u8g()
{
  u8g2.clearBuffer();	

  u8g2.setFont(u8g2_font_ncenB08_tr);
  //u8g2.drawStr(0,64,"result");
  	
  u8g2.setCursor(0,15);
  u8g2.print(stepsPerFacet);
  u8g2.setCursor(0,30);
  u8g2.print(currunt_facet);
  u8g2.setCursor(0,45);
  u8g2.print(button_prv_clicked);
  u8g2.setCursor(15,45);
  u8g2.print(button_fwd_clicked);
  u8g2.sendBuffer();	
  
  if (currunt_facet > Facet_to_Cut){
    currunt_facet = 1;
  }

  if(currunt_facet < 1) {
    currunt_facet = Facet_to_Cut;
  }
    currentscreen();
  sleep_ms(10);
}

//----------------------------------------------------------------------------------------------------------
void continuous_Run(int TMC_Speed)
{
  digitalWrite(led,HIGH);
  digitalWrite(PIN_EN, TMC_enable);
  int maxSpeed = speedMultiplyer*TMC_Speed;

  
  if((digitalRead(BUTTON_FWD_PIN) == LOW) && (button_fwd_clicked == 0)) { // unclick 
    button_fwd_clicked = 1;
    if (button_fwd_clicked = 1){
      
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
      button_fwd_clicked = 0;
    }
  }

  if((digitalRead(BUTTON_PRV_PIN) == LOW) && (button_prv_clicked == 0)) { // unclick 
    button_prv_clicked = 1;
    if (button_prv_clicked = 1){
      
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
      button_prv_clicked = 0;
    }
  }

  if((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // unclick 
    button_select_clicked = 1;
    if (button_select_clicked = 1){
      TMC_enable = !TMC_enable;
      Serial.print("Continuous Mode, Dir:");
      Serial.print(TMC_dir);
      Serial.print(", Power:");
      Serial.print(!TMC_enable);
      Serial.print(", Speed: 200* ");
      Serial.println(TMC_Speed);
      sleep_ms(250);
      button_select_clicked = 0;
    }
  }
  digitalWrite(led, LOW);
  driver.VACTUAL(maxSpeed);

}

void Homming()
{
  int Home_sens = 0;
  int Hall_sens = digitalRead(HALL_PIN);
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
void Stroking(int St_Speed , int timeGap)
{
  

  int maxSpeed = speedMultiplyer*St_Speed;
  digitalWrite(PIN_EN, LOW);

  for (int i = 0; i <= 1; i++) {
    digitalWrite(led,HIGH);
    TMC_dir = !TMC_dir;
    driver.shaft(TMC_dir);
    driver.VACTUAL(0);
    sleep_ms(10);
    digitalWrite(led,LOW);
    for (long j = 0; j <= maxSpeed; j= j+50){ // Speed up to maxSpeed
      driver.VACTUAL(j);
      sleep_ms(2);
    }
    sleep_ms(timeGap*100);
    for (long k = maxSpeed; k >= 0; k= k-50){ // Speed down to 0
      driver.VACTUAL(k);
      sleep_ms(2);
    }
  }
  driver.VACTUAL(0);
}



void backlash()
{
  stepper.move(250);
  while (stepper.distanceToGo() != 0) {
    stepper.runToPosition();
  }
  sleep_ms(100);

  ACL_Div = !ACL_Div;
  stepper.setPinsInverted(ACL_Div, false, true);
  stepper.move(250);
  while (stepper.distanceToGo() != 0) {
    stepper.runToPosition();
  }
  sleep_ms(50);
  driver.VACTUAL(0);
  sleep_ms(50);

  ACL_Div = !ACL_Div;
  stepper.setPinsInverted(ACL_Div, false, true);
}


void image360()
{
  Facet_to_Cut =7;
  stepsPerFacet = calc360Image(StepsPerRev, Facet_to_Cut);

  if((digitalRead(BUTTON_FWD_PIN) == LOW) && (button_fwd_clicked == 0)) { // unclick 
    button_fwd_clicked = 1;
    digitalWrite(led,HIGH);
    if (button_fwd_clicked = 1){
      digitalWrite(led,1);
      ACL_Div = false;
      stepper.setPinsInverted(ACL_Div, false, true);
      stepper.move(stepsPerFacet);

      while (stepper.distanceToGo() != 0){  
        stepper.runToPosition();
      }

      digitalWrite(led,0);
      sleep_ms(100);
      currunt_facet ++ ;
      sleep_ms(100);
    }
      button_fwd_clicked = 0;
      digitalWrite(led,LOW);
      driver.VACTUAL(0);
  }
  

  if((digitalRead(BUTTON_PRV_PIN) == LOW) && (button_prv_clicked == 0)) { // unclick 
    button_prv_clicked = 1;
    digitalWrite(led,HIGH);
    if (button_prv_clicked = 1){
      digitalWrite(led,1);
      ACL_Div = true;
      stepper.setPinsInverted(ACL_Div, false, true);
      stepper.move(stepsPerFacet);

      while (stepper.distanceToGo() != 0) {
        stepper.runToPosition();
      }
      backlash();

      digitalWrite(led,0);
      sleep_ms(100);
      currunt_facet -- ;
      sleep_ms(100);
    }
      button_prv_clicked = 0;
      digitalWrite(led,LOW);
      driver.VACTUAL(0);
  }
}


//----------------------------------------------------------------------------------------------------------
const int NUM_ITEMS = 5; // number of items in the list and also the number of screenshots and screenshots with QR codes (other screens)
const int MAX_ITEM_LENGTH = 20; // maximum characters for the item name

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "Select Option" }, 
  { "Stroking" }, 
  { "Continuous" }, 
  { "360_Image" }, 
  { "Settings" }, 
 };

// note - when changing the order of items above, make sure the other arrays referencing bitmaps
// also have the same order, for example array "bitmap_icons" for icons, and other arrays for screenshots and QR codes
//----------------------------------------------------------------------------------------------------------
void currentscreen(){
  //item_selected = u8g2.userInterfaceSelectionList("HomeMenu", item_selected, menu1);
  if (current_screen == 0) { // MENU SCREEN SELECTION & BUTTON CLICK

    // up and down buttons only work for the menu screen
    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
      item_selected = item_selected - 1; // select previous item
      button_up_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected < 0) { // if first item was selected, jump to last item
        item_selected = NUM_ITEMS-1;
      }
    }
    else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to next menu item
      item_selected = item_selected + 1; // select next item
      button_down_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected >= NUM_ITEMS) { // last item was selected, jump to first menu item
        item_selected = 0;
      }
    } 

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick
      button_down_clicked = 0;
    }
  }
  
  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
    button_select_clicked = 1; // set button to clicked to only perform the action once
    current_screen = 1; // menu items screen --> screenshots screen
    EnterMenuItem = item_selected;
  }

  if ((digitalRead(BUTTON_Exit_PIN) == LOW) && (button_exit_clicked == 0)) { // select button clicked, jump between screens
    button_exit_clicked = 1; // set button to clicked to only perform the action once
    current_screen = 0;// screenshots screen --> qr codes screen
    driver.VACTUAL(0);
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick 
    button_select_clicked = 0;
  }
  if ((digitalRead(BUTTON_Exit_PIN) == HIGH) && (button_exit_clicked == 1)) { // unclick 
  button_exit_clicked = 0;
  EnterMenuItem = 0;
  }
 
  // set correct values for the previous and next items
  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  item_sel_next = item_selected + 1;  
  if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} // next item would be after last = make it the first
}
//----------------------------------------------------------------------------------------------------------
void MenuSyatem(){

  currentscreen();

  u8g2.clearBuffer();  // clear buffer for storing display content in RAM

    if (current_screen == 0) { // MENU SCREEN

      // selected item background
      u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

      // draw previous item as icon + label
      u8g2.setFont(u8g_font_7x14);
      u8g2.drawStr(25, 15, menu_items[item_sel_previous]); 
      u8g2.drawXBMP( 4, 2, 16, 16, bitmap_icons[item_sel_previous]);          

      // draw selected item as icon + label in bold font
      u8g2.setFont(u8g_font_7x14B);    
      u8g2.drawStr(25, 15+20+2, menu_items[item_selected]);   
      u8g2.drawXBMP( 4, 24, 16, 16, bitmap_icons[item_selected]);     

      // draw next item as icon + label
      u8g2.setFont(u8g_font_7x14);     
      u8g2.drawStr(25, 15+20+20+2+2, menu_items[item_sel_next]);   
      u8g2.drawXBMP( 4, 46, 16, 16, bitmap_icons[item_sel_next]);  

      // draw scrollbar background
      u8g2.drawXBMP(128-8, 0, 8, 64, bitmap_scrollbar_background);

      // draw scrollbar handle
      u8g2.drawBox(125, 64/NUM_ITEMS * item_selected, 3, 64/NUM_ITEMS); 

      // draw upir logo
      u8g2.drawXBMP(128-23-4, 64-4, 23, 4, upir_logo);               
    }

    else if (current_screen == 1) { // menu SCREEN
        u8g2.setFont(u8g_font_7x14);
        //menu_page[item_selected]();
    }
    else if (current_screen == 2) { 
        current_screen = 0; 
    }
  u8g2.sendBuffer(); // send buffer from RAM to display controller
}

//----------------------------------------------------------------------------------------------------------
void loop() {

  if(current_screen == 1 && digitalRead(BUTTON_UP_PIN) == LOW) { SpeedCtrl++; sleep_ms(250); if (SpeedCtrl > speedLimit){SpeedCtrl = speedLimit;}}
  if(current_screen == 1 && digitalRead(BUTTON_DOWN_PIN) == LOW) { SpeedCtrl--; sleep_ms(250); if (SpeedCtrl < 0){SpeedCtrl = 0;}}
  if(current_screen == 1 && digitalRead(BUTTON_FWD_PIN) == LOW) { timeDelay++; sleep_ms(50); if (timeDelay > 200){timeDelay = 200;}}
  if(current_screen == 1 && digitalRead(BUTTON_PRV_PIN) == LOW) { timeDelay--; sleep_ms(50); if (timeDelay <= 1){timeDelay = 1;}}

  switch (EnterMenuItem) {
    case 0:
      MenuSyatem();
      
    break;
    case 1:
      Stroking_u8g(SpeedCtrl, timeDelay);
      HomeNeeded = 1;
      //Strok_2PT_u8g(4000);
    break;
    case 2:
      continuous_Run_u8g(SpeedCtrl);
      //continuous_Run_u8g(PotVal);
    break;
    case 3:
      image360_u8g();
      //continuous_Run_u8g(PotVal);
    break; 
    case 4:
      Settings_menu();
      //continuous_Run_u8g(PotVal);
    break;     
    default:
    break;

  }
}
//----------------------------------------------------------------------------------------------------------
void loop1() {

  // Pot = map(analogRead(26), 50, 1023, 0, 8000);
  // if (Pot <= 100){Pot=0;}PotVal = Pot/100;
  //stepper.run();
  
  switch (EnterMenuItem) {
    case 0:
    Homming();
      
    break;
    case 1:
      Stroking(SpeedCtrl, timeDelay);
      //Strok_2PT_u8g(4000);
    break;
    case 2:
      continuous_Run(SpeedCtrl);
      //continuous_Run_u8g(PotVal);
    break;
    case 3:
      image360();
      //continuous_Run_u8g(PotVal);
    break; 
    default:
      driver.VACTUAL(0);
      //stepper.run();
    break;

  }
}






