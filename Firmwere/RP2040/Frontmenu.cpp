#include "Frontmenu.h"
#include "TMC2209.h"
#include "Button.h"

//------U8G2_SH1106_Config------
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

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

void Oled_init(){
  u8g2.setColorIndex(1);  // set the color to white
  u8g2.begin();///*Select=*/ BUTTON_SELECT_PIN, /*Right/Next=*/ BUTTON_FWD_PIN, /*Left/Prev=*/ BUTTON_PRV_PIN, /*Up=*/ BUTTON_UP_PIN, /*Down=*/ BUTTON_DOWN_PIN, /*Home/Cancel=*/ BUTTON_Exit_PIN);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_6x12_tr);
  delay(300);
}

bool long_press_u8g;
char pressed_btn_u8g;
char fwd_b_u8g = 'F';
char prv_b_u8g = 'P';
char sel_b_u8g = 'S';
char up_b_u8g =  'U';
char down_b_u8g = 'D';
char exit_b_u8g = 'E';

//if(pressed_btn_u8g == fwd_b_u8g){

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
int currentscreen(){
  pressed_btn_u8g = btn_Name_Click_Out();
  //item_selected = u8g2.userInterfaceSelectionList("HomeMenu", item_selected, menu1);
if (current_screen == 0) { // MENU SCREEN SELECTION & BUTTON CLICK

    // up and down buttons only work for the menu screen
    if(pressed_btn_u8g == up_b_u8g){
    //if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
      item_selected = item_selected - 1; // select previous item
      //button_up_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected < 0) { // if first item was selected, jump to last item
        item_selected = NUM_ITEMS-1;
      }
      
    }
    else if(pressed_btn_u8g == down_b_u8g){
    //else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to next menu item
      item_selected = item_selected + 1; // select next item
      button_down_clicked = 1; // set button to clicked to only perform the action once
      if (item_selected >= NUM_ITEMS) { // last item was selected, jump to first menu item
        item_selected = 0;
      }
      
    } 

    // if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
    //   button_up_clicked = 0;
    // }
    // if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick
    //   button_down_clicked = 0;
    // }
    unclick();
  }
  
  if(pressed_btn_u8g == sel_b_u8g){
  //if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
    //button_select_clicked = 1; // set button to clicked to only perform the action once
    current_screen = 1; // menu items screen --> screenshots screen
    EnterMenuItem = item_selected;
  }

  if(pressed_btn_u8g == exit_b_u8g){
  //if ((digitalRead(BUTTON_Exit_PIN) == LOW) && (button_exit_clicked == 0)) { // select button clicked, jump between screens
    button_exit_clicked = 1; // set button to clicked to only perform the action once
    current_screen = 0;// screenshots screen --> qr codes screen
    EnterMenuItem = 0;
  }

  unclick();
  // if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick 
  //   button_select_clicked = 0;
  // }

  // if ((digitalRead(BUTTON_Exit_PIN) == HIGH) && (button_exit_clicked == 1)) { // unclick 
  //   button_exit_clicked = 0;
  // }
 
  // set correct values for the previous and next itemsEnterMenuItem
  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  item_sel_next = item_selected + 1;  
  if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} // next item would be after last = make it the first

  return current_screen;
}
//----------------------------------------------------------------------------------------------------------

int curruntMenuItem(){
  return EnterMenuItem;
}

void MenuSystem(){
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

//int tmcSpeedOut(int TMC_Speed){return TMC_Speed;}
bool TMC_dir_in = tmcDirOut();
int aVref_in = tmcVrefOut();
bool Hall_pin_in = HallOut();
int stepsPerFacet_in;// = stepsPerFacetOut();
int currunt_facet_in;// = currunt_facetOut();
int Speed_UserInput = Speed_UserInput_Out();
int timeDelay_UserInput = timeDelay_UserInput_Out();

// TMC_dir_in = tmcDirOut();
// aVref_in = tmcVrefOut();
// Hall_pin_in = HallOut();
// stepsPerFacet_in = stepsPerFacetOut();
// currunt_facet_in = currunt_facetOut();
// Speed_UserInput = Speed_UserInput_Out();
// timeDelay_UserInput = timeDelay_UserInput_Out();

void continuous_Run_u8g(){
  Speed_UserInput = Speed_UserInput_Out();
  u8g2.clearBuffer();
  u8g2.drawLine(0, 14, 128, 14);//Horizontal Top of page
  u8g2.drawLine(64, 50, 64, 63); //Vertical next to MCREEII
  u8g2.drawLine(96, 14, 96, 50); //Vertical middle of the page
  //u8g2.drawLine(64, 35, 128, 35);//Horizontal next to Direction
  u8g2.drawLine(64, 50, 128, 50);//Horizontal next to MCREEII

  u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
  u8g2.drawStr(70, 63,"McReeii");
  u8g2.setFont(u8g2_font_neuecraft_te);
  u8g2.drawStr(20, 10,"CONTINUOUS");

  u8g2.setFont(u8g2_font_6x13_mr);
 
  if(MotorStop == 0){u8g2.drawStr(58,30,"RUNNING");} else {u8g2.drawStr(68,30,"HOLD");}
  

  u8g2.setFont(u8g2_font_logisoso42_tr);
  u8g2.setCursor(0,63);
  u8g2.print(Speed_UserInput);

  u8g2.setFont(u8g2_font_open_iconic_arrow_4x_t);//u8g2_font_tenfatguys_tr);
  if(tmcDirOut() == 0){u8g2.drawGlyph(100,48,0x0051);} else {u8g2.drawGlyph(100,48,0x0052);}

  u8g2.sendBuffer();
  currentscreen();
}

void Settings_menu(){
  aVref_in = tmcVrefOut();
  Hall_pin_in = HallOut();

  u8g2.clearBuffer();	
  u8g2.setFont(u8g2_font_5x8_mr);

  u8g2.setCursor(0,8);
  u8g2.print("MOTOR CURRENT: ");
  u8g2.setCursor(80,8);
  u8g2.print(aVref_in);
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
  u8g2.print(Hall_pin_in);
  u8g2.setCursor(110,32);
  u8g2.print("0");

  u8g2.sendBuffer();
  currentscreen();
  sleep_ms(120);
}

void Stroking_u8g(){
  Speed_UserInput = Speed_UserInput_Out();
  timeDelay_UserInput = timeDelay_UserInput_Out();
  u8g2.clearBuffer();
  u8g2.drawLine(0, 14, 128, 14);//Horizontal Top of page
  u8g2.drawLine(64, 14, 64, 63); //Vertical middle of the page
  u8g2.drawLine(0, 49, 128, 49);//Horizontal next to Hold/Running

  u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
  u8g2.drawStr(70, 63,"McReeii");
  u8g2.setFont(u8g2_font_neuecraft_te);
  u8g2.drawStr(25, 10,"STROCKING");

  u8g2.setFont(u8g2_font_6x13_mr);
  if(MotorStop == 0){u8g2.drawStr(9,63,"RUNNING");} else {u8g2.drawStr(19,63,"HOLD");}

  u8g2.setFont(u8g2_font_logisoso26_tn  );
  u8g2.setCursor(10,45);
  u8g2.print(Speed_UserInput);
  u8g2.setCursor(67,45);
  u8g2.print(timeDelay_UserInput);

  u8g2.setFont(u8g2_font_tenfatguys_tr);
  if(tmcDirOut() == 0){u8g2.drawStr(119,30,"<");} else {u8g2.drawStr(119,30,">");}

  u8g2.setFont(u8g2_font_5x8_mr);
  u8g2.setFontDirection(3);
  u8g2.drawStr(6, 43, "SPEED");
  u8g2.drawStr(127, 43, "ST");
  u8g2.setFontDirection(0);
   	
  // u8g2.setFont(u8g2_font_6x12_tr);	
  // u8g2.setCursor(0,15);
  // u8g2.print("Strock: ");
  // u8g2.setCursor(40,15);
  // u8g2.print(timeDelay_UserInput);
  // u8g2.setCursor(0,30);
  // u8g2.print("Speed");
  // u8g2.setCursor(40,30);
  // u8g2.print(Speed_UserInput);
  // u8g2.setCursor(0,45);
  // u8g2.print("Press L/R for direction");
  u8g2.sendBuffer();
  currentscreen();
}
