#include "SDStepper.h"
//#include <AccelStepper.h>

// AccelStepper stepper = AccelStepper(stepper.DRIVER, PIN_STEP, PIN_DIR);

// char pressed_btn_acl;
// char fwd_b_acl = 'F';
// char prv_b_acl = 'P';
// char sel_b_acl = 'S';

// //------AccelStepper_Config------
// bool ACL_Div = 0;// Direction for AccelStepper
// long currunt_Position;
// //----------------------------------------------------------------------------------------------------------

// constexpr uint32_t StepsPerRev = ((360/MotorDeg)*10)*GearRatio;
// int Facet_to_Cut;
// int currunt_facet;

// // int stepsPerRevOut(){return StepsPerRev;}
// // int stepsPerFacetOut(){return stepsPerFacet;}
// // int currunt_facetOut(){return currunt_facet;}

// void SDStepper_init(){
//   stepper.setMaxSpeed(1000); // 3.6rev/s @ 6400steps/rev
//   stepper.setAcceleration(2000); // 2000mm/s^2
//   stepper.setEnablePin(PIN_EN);
//   stepper.setPinsInverted(false, false, true);
//   stepper.setCurrentPosition(0);
//   stepper.enableOutputs();
// }

// //------Functions------
// int calc360Image(float, float); //Steps per facet Counter
// // put function definitions here:
// int calc360Image(float StepsPerRev, float FacetToCut) {
//   return StepsPerRev / FacetToCut;
// } int stepsPerFacet;

// void backlash(){
//   digitalWrite(rpled,HIGH);
//   stepper.move(250);
//   while (stepper.distanceToGo() != 0) {
//     stepper.runToPosition();
//   }
//   sleep_ms(100);

//   ACL_Div = !ACL_Div;
//   stepper.setPinsInverted(ACL_Div, false, true);
//   stepper.move(250);
//   while (stepper.distanceToGo() != 0) {
//     stepper.runToPosition();
//   }
//   sleep_ms(50);
//   TmcStop();
//   sleep_ms(50);

//   ACL_Div = !ACL_Div;
//   stepper.setPinsInverted(ACL_Div, false, true);
//   digitalWrite(rpled,LOW);
// }
// void image360_func(){
// }

//void image360(){
  // pressed_btn_acl = btn_Name_Click_Out();
  // Facet_to_Cut =7;
  // stepsPerFacet = calc360Image(StepsPerRev, Facet_to_Cut);

  // if(pressed_btn_acl == fwd_b_acl) { // if forward btn is pressed

  //   digitalWrite(rpled,HIGH);
  //   ACL_Div = false;
  //   stepper.setPinsInverted(ACL_Div, false, true);
  //   stepper.move(stepsPerFacet);

  //   while (stepper.distanceToGo() != 0){  
  //     stepper.runToPosition();
  //   }

  //   digitalWrite(rpled,0);
  //   sleep_ms(100);
  //   currunt_facet ++ ;
  //   sleep_ms(100);

  //   pressed_btn_acl = 0;
  //   digitalWrite(rpled,LOW);
  //   TmcStop();
  // }
  
  // if(pressed_btn_acl == prv_b_acl) { // if privious btn is pressed
  //   digitalWrite(rpled,HIGH);
  //   ACL_Div = true;
  //   stepper.setPinsInverted(ACL_Div, false, true);
  //   stepper.move(stepsPerFacet);

  //   while (stepper.distanceToGo() != 0) {
  //     stepper.runToPosition();
  //   }
  //   backlash();

  //   digitalWrite(rpled,0);
  //   sleep_ms(100);
  //   currunt_facet -- ;
  //   sleep_ms(100);
  // }
  // pressed_btn_acl = 0;
  // digitalWrite(rpled,LOW);
  // TmcStop();

  // if (currunt_facet > Facet_to_Cut){
  //   currunt_facet = 1;
  // }

  // if(currunt_facet < 1) {
  //   currunt_facet = Facet_to_Cut;
  // }
  
//}
