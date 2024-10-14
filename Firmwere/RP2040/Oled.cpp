/*
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

void Settings_menu(){
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

void image360_u8g(){
  u8g2.clearBuffer();	

  u8g2.setFont(u8g2_font_ncenB08_tr);
  //u8g2.drawStr(0,64,"result");
  	
  u8g2.setCursor(0,15);
  u8g2.print(stepsPerFacet);
  u8g2.setCursor(0,30);
  u8g2.print(currunt_facet);
  u8g2.setCursor(0,45);
  u8g2.print(btn_Click_Out());
  u8g2.sendBuffer();	
  
  currentscreen();
  sleep_ms(10);
}

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

*/