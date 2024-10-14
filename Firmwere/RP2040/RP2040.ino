#include "Pins.h"
#include "TMC2209.h"
#include "Frontmenu.h"
#include "Button.h"
#include "Comm.h"
//#include "SDStepper.h"

void setup() {
  Serial.begin(115200); // Serial Port communication Via USB for serial print
  Serial.setDebugOutput(true);
  delay(500);
  Oled_init();
  Button_init();
  Tmc2209_init();
  
  Serial.println("USB Serial: Connected");
  Serial.println("TMC Driver: Connected");

  Wire1.setSDA(sda1_pin);
  Wire1.setSCL(scl1_pin);
  Wire1.begin((uint8_t)I2C_DEV_ADDR);
  Wire1.onReceive(onReceive);
  Wire1.onRequest(onRequest111);
  
  delay(1000);
}

void loop(){
  Button_func();
  UserInput();
    switch (curruntMenuItem()) {
    case 0:
      MenuSystem();
      TmcSpeed(0);
    break;
    case 1:
      Stroking_u8g();
    break;
    case 2:
      continuous_Run_u8g();
    break;
    case 3:
      //image360_func();
      //image360_u8g();
      //continuous_Run_u8g(PotVal);
    break; 
    case 4:
      Settings_menu();
    break;     
    default:
    break;
  }
  delay(10);
}
void loop1(){
  // vTaskDelay(3000); // TmcSpeed(100); // vTaskDelay(2000);// TmcSpeed(500);// vTaskDelay(2000);
  //ps();
  switch (curruntMenuItem()) {
    case 0:
    break;
    case 1:
      Stroking();
    break;
    case 2:
      continuous_Run();
    break;
    case 3:
      //image360_func();
      //image360_u8g();
      //continuous_Run_u8g(PotVal);
    break; 
    case 4:
      //continuous_Run_u8g(PotVal);
    break;     
    default:
    break;
  }
}



