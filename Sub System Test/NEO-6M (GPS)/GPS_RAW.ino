#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#define RXPin 4
#define TXPin 3
#define GPSBaud 9600

// Create GPS and Serial objects
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin); // GPS on SoftwareSerial


void setup() {
    Serial.begin(115200);
    ss.begin(GPSBaud);
    Serial.println("Checking raw GPS data...");
}

void loop() {
    while (ss.available()) {
        Serial.write(ss.read());  // Print raw NMEA sentences
    }
}
