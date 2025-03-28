#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 15
#define dio0 4
#define LED_PIN 2  // Define LED pin

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //868E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(868E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    digitalWrite(LED_PIN, HIGH);  // Turn LED on
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    digitalWrite(LED_PIN, LOW);  // Turn LED off
  }
}
