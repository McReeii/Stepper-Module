/*-----------------------------------------ESP32--Reciver--MOD-----------------------//EDIT this file Only
When new parameters needs to be add add respactive Data type to Struct **SAME AS TRANSMETERS STRUCT**.
And initialize data value by adding corresponding lines to OnDataRecv() memcopy function
*/
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

int counter = 0;
bool temp = 0;

// // Replace with your network credentials (STATION)
 const char* hostname = "MCREEII";
// const char* password = "REPLACE_WITH_YOUR_PASSWORD";

uint8_t clientMacAddress[6];

esp_now_peer_info_t slave;
int chan = 5 ; 

enum MessageType {PAIRING, DATA,};
MessageType messageType;

// Define variables to Button readings to be sent
uint8_t DataToSend_1;
uint8_t DataToSend_2;
int incomingReadingsId;

// Define variables to store incoming readings
uint8_t button_value;
uint8_t click_type;
bool err_occur;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_messagein {
  uint8_t msgType;
  uint8_t id;
  uint8_t btn_val;
  uint8_t clk_typ;
  bool err_ocr;
  unsigned int readingId;
} struct_messagein;

typedef struct struct_messageout {
  uint8_t msgType;
  uint8_t id;
  uint8_t data1;
  uint8_t data2;
  bool err_ocr;
  unsigned int readingId;
} struct_messageout;

typedef struct struct_pairing {       // new structure for pairing
  uint8_t msgType;
  uint8_t id;
  uint8_t macAddr[6];
  uint8_t channel;
} struct_pairing;

struct_messagein incomingReadings;
struct_messageout outgoingSetpoints;
struct_pairing pairingData;

void getReadings(){
  DataToSend_1 = 55;
  DataToSend_2 = 11;
}

void readDataToSend() {  //----HARD WIRED DATA------------
  outgoingSetpoints.msgType = DATA;
  outgoingSetpoints.id = 0;
  outgoingSetpoints.data1 = DataToSend_1;
  outgoingSetpoints.data2 = DataToSend_2;
  outgoingSetpoints.readingId = counter++;
}

void printResults(){  // Display Readings in Serial Monitor
  Serial.println("\nINCOMING READINGS");
  Serial.print("incoming Btn_value: ");
  Serial.print(button_value);
  Serial.println(" ºC");
  Serial.print("incoming Click_type: ");
  Serial.print(click_type);
  Serial.println(" %");
  Serial.print("incomingReadingsId: ");
  Serial.println(incomingReadingsId);
}

// ---------------------------- esp_ now -------------------------
void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

bool addPeer(const uint8_t *peer_addr) {      // add pairing
  memset(&slave, 0, sizeof(slave));
  const esp_now_peer_info_t *peer = &slave;
  memcpy(slave.peer_addr, peer_addr, 6);
  
  slave.channel = chan; // pick a channel
  slave.encrypt = 0; // no encryption
  // check if the peer exists
  bool exists = esp_now_is_peer_exist(slave.peer_addr);
  if (exists) {
    // Slave already paired.
    Serial.println("Already Paired");
    return true;
  }
  else {
    esp_err_t addStatus = esp_now_add_peer(peer);
    if (addStatus == ESP_OK) {
      // Pair success
      Serial.println("Pair success");
      return true;
    }
    else 
    {
      Serial.println("Pair failed");
      return false;
    }
  }
} 

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send: ");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Pass to " : "Fail to ");
  printMAC(mac_addr);
  Serial.println("\n");
}

void OnDataRecv(const uint8_t * mac_addr, const uint16_t *incomingData, int len) { 
  Serial.print(len);
  Serial.println(" bytes of new data received.");
  //JsonDocument root;
  String payload;
  uint8_t type = incomingData[0];       // first message byte is the type of message 
  switch (type) {
  case DATA :                           // the message is data type
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    button_value = incomingReadings.btn_val;
    click_type = incomingReadings.clk_typ;
    printResults();

  
  case PAIRING:                            // the message is a pairing request 
    memcpy(&pairingData, incomingData, sizeof(pairingData));
    Serial.print("msgType: ");
    Serial.println(pairingData.msgType);
    Serial.print("BoardID: ");
    Serial.println(pairingData.id);
    Serial.print("Pairing request from MAC Address: ");
    printMAC(pairingData.macAddr);
    Serial.print(" on channel ");
    Serial.println(pairingData.channel);

    clientMacAddress[0] = pairingData.macAddr[0];
    clientMacAddress[1] = pairingData.macAddr[1];
    clientMacAddress[2] = pairingData.macAddr[2];
    clientMacAddress[3] = pairingData.macAddr[3];
    clientMacAddress[4] = pairingData.macAddr[4];
    clientMacAddress[5] = pairingData.macAddr[5];

    if (pairingData.id > 0) {     // do not replay to server itself
      if (pairingData.msgType == PAIRING) { 
        pairingData.id = 0;       // 0 is server
        // Server is in AP_STA mode: peers need to send data to server soft AP MAC address 
        WiFi.softAPmacAddress(pairingData.macAddr);
        Serial.print("Pairing MAC Address: ");
        printMAC(clientMacAddress);
        pairingData.channel = chan;
        Serial.println(" send response");
        esp_err_t result = esp_now_send(clientMacAddress, (uint8_t *) &pairingData, sizeof(pairingData));
        addPeer(clientMacAddress);
      }  
    }  
    break; 
  }
}

void initESP_NOW(){
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
} 



void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  WiFi.setHostname(hostname);

  WiFi.mode(WIFI_AP_STA);
  WiFi.setChannel(chan);
  //WiFi.setSleep(1);
  //WiFi.mode(WIFI_STA);
  
  WiFi.STA.begin();

  Serial.print("\nServer STA MAC Address: ");
  readMacAddress();
  Serial.print("Server SOFT AP MAC Address:  ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP32 STA HostName: ");
  Serial.println(WiFi.getHostname());
  Serial.print("ESP32 Soft AP HostName: ");
  Serial.println(WiFi.softAPgetHostname());

  // Serial.print("Station IP Address: ");
  // Serial.println(WiFi.localIP());
  
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  initESP_NOW();
}

void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    //events.send("ping", NULL, millis());
    lastEventTime = millis();
    getReadings();
    readDataToSend();
    esp_now_send(NULL, (uint8_t *) &outgoingSetpoints, sizeof(outgoingSetpoints));
    temp = 1;    
  }
}