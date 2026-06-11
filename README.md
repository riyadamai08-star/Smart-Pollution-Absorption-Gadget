# Industrial LoRa Air Quality Monitor (STM32)

A high-performance, long-range environmental sensing node built on the **STM32 BlackPill** (Cortex-M4). This system tracks fine particulate matter and hazardous gases, transmitting data via **LoRa (Long Range)** telemetry. It is designed for industrial environments where Wi-Fi is unreliable or unavailable.

## 🚀 Key Features
- **ARM Cortex-M4 Processing:** High-speed data acquisition using the STM32F411CEU6.
- **Laser Particulate Sensing:** Planttower PMS5003 provides accurate PM1.0, PM2.5, and PM10 measurements.
- **Multi-Gas Detection:** MICS-6814 sensor isolates Nitrogen Dioxide (NO2), Ammonia (NH3), and Carbon Monoxide (CO).
- **Sub-GHz Telemetry:** SX1278 LoRa module enables data transmission over several kilometers.
- **Laboratory-Grade Climate Data:** SHT31-D I2C sensor for high-precision temperature and humidity tracking.
- **Automated Actuation:** Intelligent relay control for exhaust fans based on pollutant thresholds and thermal safety.

## 🛠 Hardware Stack
| Component | Function | Protocol |
| :--- | :--- | :--- |
| **STM32 BlackPill** | Main Microcontroller (F411) | - |
| **SX1278 (Ra-02)** | LoRa Transceiver (433MHz) | SPI |
| **Planttower PMS5003** | Laser Particle Counter | UART |
| **MICS-6814** | Multi-Gas Sensor | Analog |
| **SHT31-D** | High-Accuracy Temp/Hum | I2C |
| **Relay Module** | 5V Fan Control | Digital |

## 📋 Pin Mapping (STM32F411)
- **LoRa (SPI1):** `NSS: PA4`, `SCK: PA5`, `MISO: PA6`, `MOSI: PA7`, `RST: PB1`, `DIO0: PB2`
- **PMS5003 (UART1):** `TX: PA9`, `RX: PA10`
- **SHT31 (I2C1):** `SCL: PB6`, `SDA: PB7`
- **MICS-6814 (ADC):** `NO2: PA0`, `NH3: PA1`, `CO: PA2`
- **Actuator:** `Fan Relay: PB0`

## ⚙️ Setup & Installation
1. **Board Support:** Install the **STM32 Cores** in your Arduino IDE Board Manager.
2. **Library Dependencies:**
   - `LoRa` by Sandeep Mistry
   - `Adafruit SHT31 Library`
   - `PMS Library` by Mariusz Kierski
3. **Firmware:** Update the frequency in the code (`433E6` for Asia/Europe) to match your local LoRa regulations.
4. **Safety Logic:** The fan triggers automatically if $PM_{2.5} > 35 \mu g/m^3$ or if NO2 levels spike.

## 📡 Data Packet Format
Data is transmitted as a serialized string over LoRa:
`T:[Temp]|H:[Hum]|PM:[PM2.5]|NO2:[Raw]|CO:[Raw]`

