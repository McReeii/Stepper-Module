#ifndef Pins_h
#define Pins_h

//------Stepper_PINS--------------------------------------------------------
#define VREF            26 // analog0 pin 26 for vRef Masurement
#define PIN_INDEX       24 //Indexing
#define PIN_DIAG        25 // diagnostic
#define PIN_EN          20 // Enable (inverted) 
#define PIN_DIR         22 // direction
#define PIN_STEP        23 // Step
//#define PIN_RX        1 // RX
//#define PIN_TX        0 // TX

//------TMCStepper_Config--------------------------------------------------------
#define TMC_Serial      Serial1  // Serial1
#define DRIVER_ADDRESS  0b00  // MS1 and MS2 = address
#define STALL_VALUE     100  // [0...255]
#define R_SENSE         0.11f // Match to your driver // SilentStepStick series use 0.11
#define RMS_I           950 // Match to your driver // SilentStepStick series use 0.11
#define DRIVER_ADDRESS  0b00 // TMC2209 driver address when MS1 LOW and MS2 LOW
#define MICROSTEPS      16

//------Input Button_Config--------------------------------------------------------
#define rpled             2
#define BUTTON_SELECT_PIN 14 // pin for Select Button
#define BUTTON_Exit_PIN   6 // pin for Exit button
#define BUTTON_FWD_PIN    3 // pin for NEXT button
#define BUTTON_DOWN_PIN   13 // pin for DOWN button
#define BUTTON_PRV_PIN    18 // pin for BACK button 
#define BUTTON_UP_PIN     7 // pin for UP button                                          

//------Hall_Sensor_Config--------------------------------------------------------
#define HALL_PIN          27 //Hall Sensor input (digital)

//------Stepper_Motor_Config------------------------------------------------------
#define GearRatio         48 //GearRatio:1 i.e (48:1 GearRatio)
#define MotorDeg          18 // Motor Degree Multiplied by 10 i.e. (1.8 deg = 18)
#define speedLimit        200 //max speed limit
#define MaxTimeDelay      200 //max time delay
#define speedMultiplyer   200

//------I2C_Esp32_Inputs------------------------------------------------------
#define I2C_DEV_ADDR 0x55
#define sda1_pin 10
#define scl1_pin 11

//------I2C_Oled_Oututs------------------------------------------------------
// #define I2C_Oled_ADDR 0x3D
// #define sda0_pin 10
// #define scl0_pin 11



#endif
