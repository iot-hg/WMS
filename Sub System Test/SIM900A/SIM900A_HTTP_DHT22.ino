#include <HardwareSerial.h>
#include <DHT.h>

HardwareSerial sim900(1);  // Use Serial1 for SIM900A (RX=16, TX=17)
#define DHTPIN 4  
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Your SIM Card APN settings (Change for your network provider)
const char* apn = "m2mautotronic";      // Example: "internet"
const char* user = "";             // Leave empty if not required
const char* pass = "";             // Leave empty if not required

const char* thingsboardServer = "http://demo.thingsboard.io/api/v1/SKgQldR1HaUTNoA40Mwg/telemetry";  // ThingsBoard API URL

void setup() {
  Serial.begin(115200);
  sim900.begin(9600, SERIAL_8N1, 16, 17);  // SIM900 TX=16, RX=17
  dht.begin();
  Serial.println("Initializing SIM900...");
  sendCommand("AT", "OK", 2000);
  sendCommand("AT+CPIN?", "READY", 5000);
  sendCommand("AT+CREG?", "0,1", 5000);
  sendCommand("AT+CGATT?", "1", 5000);
  sendCommand("AT+CIPSHUT", "OK", 5000);
  sendCommand("AT+CSTT=\"" + String(apn) + "\",\"" + String(user) + "\",\"" + String(pass) + "\"", "OK", 5000);
  sendCommand("AT+CIICR", "OK", 5000);
  sendCommand("AT+CIFSR", ".", 5000);
  Serial.println("Connected to GPRS!");
}

void loop() {
//  float temperature = random(200, 350) / 10.0;  // Temperature (20.0 - 35.0 °C)
//  float humidity = random(400, 700) / 10.0;  // Humidity (40.0 - 70.0 %)
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
   if (isnan(humidity) || isnan(temperature) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  String jsonData = "{\"temperature\":" + String(temperature) + ", \"humidity\":" + String(humidity) + "}";

  Serial.println("Sending data: " + jsonData);
  sendHTTPPOST(thingsboardServer, jsonData);
  
  delay(10000);  // Send data every 10 seconds
}

void sendHTTPPOST(String server, String payload) {
  sendCommand("AT+CIPSTART=\"TCP\",\"" + getHost(server) + "\",80", "CONNECT OK", 5000);
  sendCommand("AT+CIPSEND", ">", 3000);
  String httpRequest = "POST " + getPath(server) + " HTTP/1.1\r\n";
  httpRequest += "Host: " + getHost(server) + "\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: " + String(payload.length()) + "\r\n";
  httpRequest += "\r\n" + payload;
  sim900.print(httpRequest);
  delay(3000);
  sim900.write(26);  // End of message (CTRL+Z)
  delay(5000);
  Serial.println("Data sent!");

  sendCommand("AT+CIPCLOSE", "OK", 5000);
}

void sendCommand(String command, String expectedResponse, int timeout) {
  sim900.println(command);
  long int time = millis();
  while ((millis() - time) < timeout) {
    if (sim900.available()) {
      String response = sim900.readString();
      Serial.println(response);
      if (response.indexOf(expectedResponse) != -1) break;
    }
  }
}

String getHost(String url) {
  int start = url.indexOf("//") + 2;
  int end = url.indexOf("/", start);
  return url.substring(start, end);
}

String getPath(String url) {
  int start = url.indexOf("//") + 2;
  start = url.indexOf("/", start);
  return url.substring(start);
}
