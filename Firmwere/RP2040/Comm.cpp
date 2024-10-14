#include "Comm.h"

bool btn_click_i2c;
uint8_t click_out;
char click_name_out;
uint8_t click_type_out;
uint16_t hold_ms_out;
char i2c_btn_cmdtype;


void onReceive(int len){
  btn_click_i2c = true;
  Serial.printf("i2c_1_rcv[%d]: ", len);

  while(4 < Wire1.available()){ //leave 4 bytes and read the rest
    hold_ms_out = Wire1.read();
    Serial.print(hold_ms_out); //string
  }
  Serial.print(" >> ");

  while(3 < Wire1.available()){ //leave 3 bytes and read the rest
    click_name_out = Wire1.read();
    Serial.print(click_name_out);  // bool
  }
  Serial.print(" >> ");

  while(2 < Wire1.available()){ //leave 2 bytes and read the rest
    click_type_out = Wire1.read();
    Serial.print(click_type_out);  // bool
  }
  Serial.print(" >> ");

  while(1 < Wire1.available()){ //leave 1 bytes and read the rest
    click_out = Wire1.read();
    Serial.print(click_out);  // bool
  }
  Serial.print(" >> ");

  while(Wire1.available()){ //read all bytes left that's available
    i2c_btn_cmdtype = Wire1.read();
    Serial.println(i2c_btn_cmdtype); //int
  }

}

uint32_t i = 0; //4 bytes
void onRequest111(){
  Wire1.print("Packets.");
  Wire1.print(i++);
  Wire1.print("----");
  //Serial.println("onRequest");
}
