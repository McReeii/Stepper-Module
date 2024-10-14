/*-----------------------------------------ESP8266--Transmitter--MOD-----------------------//EDIT this file Only-----No Err
When new parameters needs to be add respective Data types to Struct **SAME AS RECEIVER STRUCT**.
And initialize data value by adding corresponding lines to OnDataRecv() memcopy function
*/
#include "EspNow8266.h"

uint8_t channel = 5;
int readingId = 0;

//#define MAX_CHANNEL 13  // 11 in North America or 13 in Europe

unsigned long start;
unsigned long currentMillis = millis(); 
unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

unsigned long previousMillis = 0;    // will store last time Data was updated 

uint8_t broadcastAddressX[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t clientMacAddress[6];

enum PairingStatus {PAIR_REQUEST, PAIR_REQUESTED, PAIR_PAIRED, };
PairingStatus pairingStatus = PAIR_REQUEST;

bool Device_Pair_Status = 0;

enum MessageType {PAIRING, DATA,};
MessageType messageType;

// Define variables to Button readings to be sent
uint8_t button_value;
uint8_t click_type;
bool err_ocr;

// Define variables to store incoming readings
uint8_t incomingData1;
uint8_t incomingData2;
int incomingReadingsId;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_messagein {
  uint8_t msgType;
  uint8_t id;
  uint8_t data1;
  uint8_t data2;
  bool err_ocr;
  unsigned int readingId;
} struct_messagein;

typedef struct struct_messageout {
  uint8_t msgType;
  uint8_t id;
  uint8_t btn_val;
  uint8_t clk_typ;
  bool err_ocr;
  unsigned int readingId;
} struct_messageout;

typedef struct struct_pairing { // new structure for pairing
  uint8_t msgType;
  uint8_t id;
  uint8_t macAddr[6];
  uint8_t channel;
} struct_pairing;

// Create a struct_message called myData
struct_messageout myData;
struct_messagein incomingReadings;
struct_pairing pairingData;

void getReadings(uint8_t btn_val, uint8_t clk_typ){
  button_value = btn_val;
  click_type = clk_typ;
}

void readDataToSend(){
  myData.msgType = DATA;
  myData.id = 2;
  myData.btn_val = button_value;
  myData.clk_typ = click_type;
  myData.readingId = readingId ++;
}

//--------------For Accecing Incoimg Data-------------

uint8_t readData1() {
  uint8_t Data_Val = incomingData1;
  return Data_Val;
}

uint8_t readData2() {
  uint8_t Data_Val = incomingData2;
  return Data_Val;
}

bool err_occred() {
  bool err_Val = err_ocr;
  return err_Val;
}


//--------------For Accecing Incoimg Data-------------END


void printIncomingReadings(){
  // Display Readings in Serial Monitor
  Serial.println("\nINCOMING READINGS");
  Serial.print("incoming Data 1: ");
  Serial.print(incomingData1);
  Serial.println(" ºC");
  Serial.print("incoming Data 2: ");
  Serial.print(incomingData2);
  Serial.println(" %");
  Serial.print("Led: ");
  Serial.print(incomingReadingsId);
}

// ---------------------------- esp_ now -------------------------
void readGetMacAddress(){
  String val = WiFi.macAddress();
  Serial.println(val);
  char* endPtr; 
  clientMacAddress[0] = strtol(val.c_str(), &endPtr, 16); // read the first starting at the beginning of the buffer. this initializes endPtr as a pointer to the ':' after the first number 
  for (int i = 1;  (*endPtr) && (i < 6); i++) {
    clientMacAddress[i] = strtol(endPtr + 1, &endPtr, 16); // using +1 for the pointer as we want to skip the ':'
  }

  for (int i = 0; i < 6; i++) {
    Serial.print(clientMacAddress[i], HEX);
    if (i != 5) Serial.print(F(":"));
  }
}

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send: ");
  if (sendStatus == 0){
    Serial.println("Pass");
  }
  else{
    Serial.print("Fail");
    Serial.println(" ... Connection Lost");

  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  Serial.print("\nSize of message : ");
  Serial.print(len);
  Serial.print(" from ");
  printMAC(mac);
  Serial.println();
  uint8_t type = incomingData[0];
  switch (type) {
  case DATA :  
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    incomingData1 = incomingReadings.data1;
    incomingData2 = incomingReadings.data2;
    printIncomingReadings();
    
    // if (incomingReadings.readingId % 2 == 1){
    //     digitalWrite(LED_BUILTIN, LOW);
    //   } else { 
    //     digitalWrite(LED_BUILTIN, HIGH);
    //   }
    break;

  case PAIRING:
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    if (pairingData.id == 0) {                // the message comes from server
      Serial.print("Pairing done for ");
      printMAC(pairingData.macAddr);
      Serial.print(" on channel " );
      Serial.print(pairingData.channel);    // channel used by the server
      Serial.print(" in ");
      Serial.print(millis()-start);
      Serial.println("ms");
      //esp_now_del_peer(pairingData.macAddr);
      //esp_now_del_peer(mac);
      esp_now_add_peer(pairingData.macAddr, ESP_NOW_ROLE_COMBO, pairingData.channel, NULL, 0); // add the server to the peer list 
      pairingStatus = PAIR_PAIRED ;            // set the pairing status
      Device_Pair_Status = true;
      EspNowSend(); //Send first One Msg to verify connection...Should Show on Recivers Serial Port
    }
    break;
  }  
}

PairingStatus autoPairing(){
  switch(pairingStatus) {
  case PAIR_REQUEST:
    Serial.print("\nPairing requested on channel "  );
    Serial.println(channel);
    Device_Pair_Status = false;
  
    // clean esp now
    esp_now_deinit();
    WiFi.mode(WIFI_STA);
    // set WiFi channel   
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
    //WiFi.printDiag(Serial);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    // set callback routines
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // set pairing data to send to the server
    pairingData.id = BOARD_ID;     
    pairingData.channel = channel;
    pairingData.macAddr[0] = clientMacAddress[0];
    pairingData.macAddr[1] = clientMacAddress[1];
    pairingData.macAddr[2] = clientMacAddress[2];
    pairingData.macAddr[3] = clientMacAddress[3];
    pairingData.macAddr[4] = clientMacAddress[4];
    pairingData.macAddr[5] = clientMacAddress[5];
    previousMillis = millis();
    // add peer and send request
    //Serial.println(esp_now_send(broadcastAddressX, (uint8_t *) &pairingData, sizeof(pairingData)));   //Statement responds if msg is received or Not, (0 = not receivrd, 1 = Recevied)
    esp_now_send(broadcastAddressX, (uint8_t *) &pairingData, sizeof(pairingData));
    pairingStatus = PAIR_REQUESTED;
    break;

  case PAIR_REQUESTED:
    // time out to allow receiving response from server
    //currentMillis = millis();
    // if(currentMillis - previousMillis > 3000) {
    //   previousMillis = currentMillis;
         //time out expired,  try next channel
    //   channel ++;
    //   if (channel > MAX_CHANNEL) {
    //     channel = 0;
    //   }
       pairingStatus = PAIR_REQUEST; 
    // }
    break;

  case PAIR_PAIRED:
    //Serial.println("Paired!");
    break;
  }
  return pairingStatus;
} 

void EspNow8266_init(){
  pairingData.id = BOARD_ID;
  //pinMode(LED_BUILTIN, OUTPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  readGetMacAddress();
  //Serial.println(WiFi.macAddress());
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
}

bool EspNowAutoPair(){
  currentMillis = millis();
    if(currentMillis - previousMillis > 2000) {
      previousMillis = currentMillis;
      while(autoPairing() == !PAIR_PAIRED){}
    }
  bool Device_paired = Device_Pair_Status;
  return Device_paired;
}

void EspNowSend(){
  if (autoPairing() == PAIR_PAIRED) { 
    readDataToSend();
    esp_now_send(pairingData.macAddr, (uint8_t *) &myData, sizeof(myData));
  }
}

 
// void EspNow8266_func(){ 
//   if (autoPairing() == PAIR_PAIRED) { 
//     static unsigned long lastEventTime = millis();
//     static const unsigned long EVENT_INTERVAL_MS = 10000;
//     if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
//       Serial.print("...");
//       //getReadings();
      
//       //------Set-values-to-send---- ---pulled-from-btn-readins--DATA------------
//       readDataToSend();
   
//       // Send message via ESP-NOW to all peers 
//       esp_now_send(pairingData.macAddr, (uint8_t *) &myData, sizeof(myData));
//       lastEventTime = millis();
//     }
//   }
// }