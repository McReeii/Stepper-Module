#include "Comm.h"

void EspSerial(){

Serial.begin(74880);
while (!Serial) yield();
}