#include "onrecive.h"

void onReceive(int len){
  Serial.printf("onReceive[%d]: ", len);
  bool  qq;
  int   tt;

  while(2 < Wire1.available()){ //leave 2 bytes and read the rest
    char indexer = Wire1.read();
    Serial.print(indexer); //string
  }
  Serial.print(">>>>");

  while(1 < Wire1.available()){ //leave 1 bytes and read the rest
  qq = Wire1.read();
  Serial.print(qq);  // bool
  }
  Serial.print(">>>>");

  while(Wire1.available()){ //read all bytes left that's available
    tt = Wire1.read();
    Serial.println(tt); //int
  }
}


uint32_t i = 0;

void onRequest111(){
  Wire1.print("Packets.");
  Wire1.print(i++);
  Wire1.print("----");
  //Serial.println("onRequest");
}
