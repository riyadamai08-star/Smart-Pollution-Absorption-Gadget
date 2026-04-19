#define BLYNK_TEMPLATE_ID "TMPL3uzjcFJv_"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "bqkLKIlZBisNbYUsAt83JU-oYoNKwdH-"

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ---------- WiFi Credentials ----------
char ssid[] = "ADYPSOE";
char pass[] = "adypsoe@123";

// ---------- LoRa Pins (ESP32) ----------
#define SCK     18
#define MISO    19
#define MOSI    23
#define SS      5
#define RST     14
#define DIO0    2

void setup() {
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) { // Must match STM32 Frequency
    Serial.println("LoRa Receiver Init Failed");
    while (1);
  }
  Serial.println("LoRa Gateway Ready. Waiting for data...");
}

void loop() {
  Blynk.run();

  // Check for LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    
    Serial.println("Received: " + incoming);
    parseAndUpload(incoming);
  }
}

// Function to split the string T:25.5|H:60|PM:12... and send to Blynk
void parseAndUpload(String data) {
  // Example format: T:25.50|H:60.00|PM:12|NO2:450|CO:300
  
  float temp = getValue(data, 'T', '|').toFloat();
  float hum  = getValue(data, 'H', '|').toFloat();
  int pm25   = getValue(data, 'P', '|').toInt(); // Adjusted to look for PM
  int no2    = getValue(data, 'N', '|').toInt(); // Looks for NO2
  int co     = getValue(data, 'C', '|').toInt(); // Looks for CO

  // Update Blynk Virtual Pins
  Blynk.virtualWrite(V2, temp);   // Matches your previous Blynk setup
  Blynk.virtualWrite(V13, hum);
  Blynk.virtualWrite(V11, pm25);  // PM2.5 in place of AQI
  Blynk.virtualWrite(V7, no2);
  Blynk.virtualWrite(V8, co);
  
  // Update Terminal
  Blynk.virtualWrite(V15, "Received Node Data: PM2.5=" + String(pm25));
}

// Helper function to extract values between identifiers
String getValue(String data, char separator, char endChar) {
  int start = data.indexOf(separator);
  if (start == -1) return "";
  start += 2; // Move past 'T:' or 'H:'
  int end = data.indexOf(endChar, start);
  if (end == -1) end = data.length();
  return data.substring(start, end);
}
