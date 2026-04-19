# Smart-Pollution-Absorption-Gadget
An ESP32-powered IoT Air Quality monitor that calculates filtration efficiency and automates fan control based on gas levels, humidity, and thermal safety limits.

# IoT Air Quality Monitoring & Automated Filtration System

An ESP32-based environmental monitoring solution that tracks air pollutants in real-time and manages air flow via an automated fan system. This project integrates multiple sensors to provide a comprehensive view of indoor air health through the Blynk IoT platform.

## 🌟 Key Features
- **Pollutant Tracking:** Monitors levels of Ammonia, NOx, Benzene, Sulfide, and Smoke using the MQ135 sensor.
- **Dual-Zone Temperature:** Simultaneously reads ambient temperature (DHT11) and hardware/waterproof temperature (DS18B20).
- **Smart Logic:** Automated fan control that adjusts its trigger threshold based on humidity levels.
- **Safety Protocol:** Hardware protection layer that force-stops the fan/system if temperatures exceed 50°C.
- **Remote Dashboard:** Full mobile control with manual override and real-time data visualization via Blynk.

## 🛠 Hardware Components
- **Microcontroller:** ESP32 (NodeMCU)
- **Air Quality:** MQ135 Gas Sensor
- **Ambient Sensors:** DHT11 (Temp & Humidity)
- **Precision Temp:** DS18B20 (OneWire)
- **Actuators:** 5V Relay Module & DC Exhaust Fan

## 📊 Data Mapping (Blynk Virtual Pins)
| Pin | Description | Pin | Description |
| :--- | :--- | :--- | :--- |
| **V0** | Inlet Raw Data | **V8** | Benzene Level |
| **V2** | DS18B20 Temp | **V11** | Inlet AQI |
| **V3** | Filter Efficiency % | **V13** | Humidity % |
| **V4** | Manual Fan Toggle | **V14** | DHT11 Temp |
| **V6** | Ammonia Level | **V15** | System Terminal |

## 🚀 Getting Started
1. **Libraries:** Install `Blynk`, `DHT`, `OneWire`, and `DallasTemperature` via Arduino Library Manager.
2. **Configuration:** Enter your WiFi credentials and Blynk Auth Token in the main `.ino` file.
3. **Calibration:** For better AQI accuracy, ensure the MQ135 has been "burned-in" for at least 24 hours.
