#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <PMS.h>

// ---------- Pin Definitions (BlackPill) ----------
#define FAN_PIN      PB0
#define MICS_NO2     PA0
#define MICS_NH3     PA1
#define MICS_CO      PA2

// LoRa Pins (SPI1)
#define SCK     PA5
#define MISO    PA6
#define MOSI    PA7
#define SS      PA4
#define RST     PB1
#define DIO0    PB2

// ---------- Sensors ----------
Adafruit_SHT31 sht31 = Adafruit_SHT31();
PMS pms(Serial1); // PMS5003 on TX1/RX1 (PA9/PA10)
PMS::DATA data;

// ---------- Variables ----------
unsigned long lastSendTime = 0;
const int interval = 5000; // 5 seconds

void setup() {
  Serial.begin(115200);   // USB Debug
  Serial1.begin(9600);    // PMS5003

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH); // Relay OFF

  // Initialize SHT31
  if (!sht31.begin(0x44)) {
    Serial.println("Could not find SHT31 sensor!");
  }

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) { // Set frequency (433E6, 868E6, or 915E6)
    Serial.println("LoRa init failed!");
    while (1);
  }
  
  Serial.println("STM32 LoRa Node Initialized");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    readAndSendData();
    lastSendTime = millis();
  }
}

void readAndSendData() {
  // 1. Read SHT31 (I2C)
  float temp = sht31.readTemperature();
  float hum = sht31.readHumidity();

  // 2. Read MICS6814 (Analog)
  int no2 = analogRead(MICS_NO2);
  int nh3 = analogRead(MICS_NH3);
  int co  = analogRead(MICS_CO);

  // 3. Read PMS5003 (UART)
  int pm25 = 0;
  if (pms.read(data)) {
    pm25 = data.PM_AE_UG_2_5;
  }

  // 4. Fan Logic
  // Trigger fan if PM2.5 > 35 or NO2 is high
  if (pm25 > 35 || no2 > 2000) {
    digitalWrite(FAN_PIN, LOW); // Fan ON
  } else {
    digitalWrite(FAN_PIN, HIGH); // Fan OFF
  }

  // 5. Transmit via LoRa
  Serial.print("Sending packet...");
  
  LoRa.beginPacket();
  LoRa.print("T:"); LoRa.print(temp);
  LoRa.print("|H:"); LoRa.print(hum);
  LoRa.print("|PM:"); LoRa.print(pm25);
  LoRa.print("|NO2:"); LoRa.print(no2);
  LoRa.print("|CO:"); LoRa.print(co);
  LoRa.endPacket();

  Serial.println(" Done.");
}
