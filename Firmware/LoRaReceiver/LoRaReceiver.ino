#include <SPI.h>
#include "LoRa.h"

//#define DEBUG

int packetSize;
String outputStr;
char nextCh;

void setup() {
  Serial.begin(9600);
  while (!Serial);

#ifdef DEBUG
  Serial.println("LoRa Receiver");  // LinkIt starts working...
#endif

  if (!LoRa.begin(433E6)) {

#ifdef DEBUG
    Serial.println("Starting LoRa failed!");  // LoRa initialization failed...
#endif

    while (1);
  }
}

void loop() {
  packetSize = LoRa.parsePacket();  // try to parse packet
  if (!packetSize) {
    return;
  }
  
  while (LoRa.available()) {  // read packet
    nextCh = (char)LoRa.read();

    if (nextCh != '\n') {  // checks EndOfLine
      outputStr += nextCh;

    } else {
      if (outputStr.startsWith("SOS:")) {  // filters out noise received by LoRa
        outputStr.replace("SOS:","");
        Serial.println(outputStr);
      }
      outputStr = "";
      break;
    }
  }
  return;
}
