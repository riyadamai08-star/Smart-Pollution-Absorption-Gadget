// This code is based for our ESP32 prototype
#define BLYNK_TEMPLATE_ID "TMPL3uzjcFJv_"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "bqkLKIlZBisNbYUsAt83JU-oYoNKwdH-"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// ---------- WiFi Credentials ----------
char ssid[] = "riya";
char pass[] = "987654321";

// ---------- Pins ----------
#define MQ135_OUTLET 34
#define MQ135_INLET  32
#define ONE_WIRE_BUS 5
#define DHTPIN 18
#define FAN_PIN 4

// ---------- Sensor Objects ----------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds(&oneWire);

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

// ---------- Global Variables ----------
int manualOverride = 0;
int manualState = 0;

// ---------- Helper: Smoothing Function ----------
float smooth(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10.0;
}

// ---------- Helper: AQI Calculation ----------
int calculateAQI(float rawValue) {
  int aqi = map((int)rawValue, 0, 4095, 0, 500);
  return constrain(aqi, 0, 500);
}

// ---------- Helper: Relay Control ----------
void setRelay(bool on) {
  digitalWrite(FAN_PIN, on ? LOW : HIGH); // Assuming Low-Level Trigger Relay
}

// ---------- Logic: Fan Control ----------
void controlFan(float inletRaw, float humidity) {
  if (manualOverride) {
    setRelay(manualState == 1);
    return;
  }

  float threshold = 1500;
  if (humidity > 70) threshold *= 1.1;
  if (humidity < 30) threshold *= 0.9;

  setRelay(inletRaw > threshold);
}

// ---------- Blynk Virtual Pin Writes ----------
BLYNK_WRITE(V4) { // Manual Toggle (1 = ON, 0 = OFF)
  manualOverride = 1;
  manualState = param.asInt();
}

BLYNK_WRITE(V5) { // Reset to Auto Mode
  manualOverride = 0;
}

// ---------- Main Data Function ----------
void sendData() {
  float inletRaw  = smooth(MQ135_INLET);
  float outletRaw = smooth(MQ135_OUTLET);

  ds.requestTemperatures();
  float tempDS = ds.getTempCByIndex(0);

  float dhtTemp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(dhtTemp) || isnan(humidity)) {
    Serial.println("❌ DHT Error");
    return;
  }

  int aqiInlet  = calculateAQI(inletRaw);
  int aqiOutlet = calculateAQI(outletRaw);

  float efficiency = 0;
  if (inletRaw > 0) {
    efficiency = (1 - (outletRaw / inletRaw)) * 100.0;
  }
  efficiency = constrain(efficiency, 0, 100);

  // Gas Component Estimates
  float ammonia = inletRaw * 0.25;
  float nox     = inletRaw * 0.20;
  float benzene = inletRaw * 0.15;
  float sulfide = inletRaw * 0.18;
  float smoke   = inletRaw * 0.22;

  // Fan Control & Safety
  controlFan(inletRaw, humidity);
  if (tempDS > 50 || dhtTemp > 45) {
    setRelay(false);
    Blynk.logEvent("overheat", "Temperature exceeded safety limit!");
  }

  // Update Blynk App
  Blynk.virtualWrite(V0, inletRaw);
  Blynk.virtualWrite(V1, outletRaw);
  Blynk.virtualWrite(V2, tempDS);
  Blynk.virtualWrite(V3, efficiency);
  Blynk.virtualWrite(V6, ammonia);
  Blynk.virtualWrite(V7, nox);
  Blynk.virtualWrite(V8, benzene);
  Blynk.virtualWrite(V9, sulfide);
  Blynk.virtualWrite(V10, smoke);
  Blynk.virtualWrite(V11, aqiInlet);
  Blynk.virtualWrite(V12, aqiOutlet);
  Blynk.virtualWrite(V13, humidity);
  Blynk.virtualWrite(V14, dhtTemp);

  // Terminal Output
  String fullData = "AQI Inlet: " + String(aqiInlet) + " | Efficiency: " + String(efficiency,1) + "%";
  Blynk.virtualWrite(V15, fullData);
}

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);
  setRelay(false);
  
  ds.begin();
  dht.begin();
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, sendData);
}

void loop() {
  Blynk.run();
  timer.run();
}
