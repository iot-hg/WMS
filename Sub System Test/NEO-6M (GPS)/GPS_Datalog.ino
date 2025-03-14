#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define RXPin 4
#define TXPin 3
#define GPSBaud 9600
#define chipSelect 10
#define LOG_INTERVAL 600000  // 10 minutes in milliseconds
#define MAX_ENTRIES (3 * 24 * 6)  // 3 days * 24 hours * 6 logs per hour

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
File logFile;
unsigned long lastLogTime = 0;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(chipSelect)) {
    Serial.println(F("❌ SD card initialization failed!"));
    return;
  }
  Serial.println(F("✅ SD card ready."));
}

void loop() {
  while (ss.available()) gps.encode(ss.read());

  if (millis() - lastLogTime >= LOG_INTERVAL && gps.location.isValid() && gps.time.isValid()) {
    logGPSData();
    lastLogTime = millis();
  }
}

// Function to log GPS data to SD card and print it to Serial
void logGPSData() {
  logFile = SD.open("gps_log.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println(F("❌ Error opening SD file!"));
    return;
  }

  String timestamp = getFormattedTime();
  String logEntry = timestamp + ", " +
                    String(gps.location.lat(), 6) + ", " +
                    String(gps.location.lng(), 6) + ", " +
                    String(gps.altitude.meters(), 2) + ", " +
                    String(gps.speed.kmph(), 2);

  logFile.println(logEntry);
  logFile.close();

  Serial.println("✅ Data logged: " + logEntry);

  manageLogFile();
  printLogFile();
}

// Function to format timestamp with UTC+7 conversion
String getFormattedTime() {
  int hour = gps.time.hour() + 7;
  if (hour >= 24) hour -= 24;  

  char buffer[20];
  sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
          gps.date.day(), gps.date.month(), gps.date.year(),
          hour, gps.time.minute(), gps.time.second());
  return String(buffer);
}

// Function to limit log file size to 3 days
void manageLogFile() {
  logFile = SD.open("gps_log.txt", FILE_READ);
  if (!logFile) return;

  String lines[MAX_ENTRIES];
  int count = 0;

  while (logFile.available() && count < MAX_ENTRIES) {
    lines[count++] = logFile.readStringUntil('\n');
  }
  logFile.close();

  if (count < MAX_ENTRIES) return;  // Do nothing if within limit

  logFile = SD.open("gps_log.txt", FILE_WRITE);
  for (int i = count - MAX_ENTRIES + 1; i < count; i++) {
    logFile.println(lines[i]);
  }
  logFile.close();
}

// Function to print log file content to Serial
void printLogFile() {
  logFile = SD.open("gps_log.txt", FILE_READ);
  if (!logFile) {
    Serial.println(F("❌ Error opening SD file for reading!"));
    return;
  }

  Serial.println(F("📜 GPS Log File Contents:"));
  while (logFile.available()) {
    Serial.println(logFile.readStringUntil('\n'));
  }
  logFile.close();
}
