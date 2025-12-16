# Project: VARTA — Vital Agricultural Real-Time Analytics 

A comprehensive soil monitoring solution built with the Frugal-IoT framework for ESP32/ESP8266 microcontrollers. This project monitors soil moisture, ambient temperature/humidity, and soil temperature with wireless connectivity and control capabilities.

## Features

- **Soil Moisture Monitoring**: Analog sensor with configurable range (0-100%)
- **Environmental Sensing**: SHT30 or SHT40 temperature and humidity sensor via I2C
- **Soil Temperature**: DS18B20 waterproof temperature sensor
- **Battery Monitoring**: Voltage divider circuit for battery voltage measurement
- **Wireless Connectivity**: WiFi with MQTT publishing
- **Multi-Board Support**: Compatible with various ESP32/ESP8266 development boards
- **Power Management**: Configurable sleep modes for battery operation
- **LoRa Mesh**: Optional LoRaMesher support on compatible boards

## Hardware Requirements

### Core Components
- ESP32 or ESP8266 development board
- Capacitive Soil moisture sensor
- SHT30 or SHT40 temperature/humidity sensor
- DS18B20 waterproof temperature sensor
- Voltage divider circuit (2x 100kΩ resistors for 1:2 ratio battery monitoring)

### Voltage Divider Circuit
The battery voltage is measured using a simple voltage divider with two 100kΩ resistors in series. This creates a 1:2 voltage ratio, meaning 4.0V at the battery terminal is seen as 2.0V at the ESP ADC pin, keeping it within the safe 0-3.3V range.

### Supported Boards

The system is tested on the nodemcu-32s such as:  **[link](https://robocraze.com/products/nodemcu-32-wifi-bluetooth-esp32-development-board30-pin?_pos=3&_psq=esp32&_ss=e&_v=1.0)**

It should work on the following boards, with pin adjustments, but is not yet tested. 

- **ESP32-C3**: Lolin C3 Pico, C3 Mini
- **ESP32-S2**: Lolin S2 Mini  
- **ESP8266**: D1 Mini, D1 Mini Pro
- **LoRa Boards**: TTGO LoRa32 v2.1, LilyGo T3-S3

## Pin Configuration

These pins are specific to the Node MCU board

| Component | Pin |
|-----------|-----|
| Soil Moisture | GPIO 32 |
| DS18B20 | GPIO 5 | 
| SHT40 | I2C (SDA → GPIO 21, SCL → GPIO 22) |
| Battery Voltage (Voltage Divider) | GPIO 35 (via 2x 100kΩ divider) |

## Build & Installation Guide

This guide details how to set up a fresh development environment and build the VARTA firmware.

### 1. Prerequisites
To build this project, you need **Visual Studio Code** with the **PlatformIO IDE** extension.

1.  **Install VS Code**: Download and install from [code.visualstudio.com](https://code.visualstudio.com/).
2.  **Install PlatformIO**:
    * Open VS Code.
    * Go to the Extensions view (click the square icon on the left sidebar or press `Ctrl+Shift+X`).
    * Search for "PlatformIO IDE".
    * Click **Install**.
    * *Note: Wait for the installation to complete. It may prompt you to restart VS Code.*

### 2. Setup the Repository
1.  **Clone the Repo** (for this git must be installed on you machine):
    ```bash
    git clone [https://github.com/tarun455/iot-gdt.git](https://github.com/tarun455/iot-gdt.git)
    ```
2.  **Open in VS Code**:
    * Open VS Code.
    * Click **File > Open Folder...**
    * Select the `iot-gdt` folder you just cloned.
3.  **Initialization**:
    * PlatformIO will automatically detect the `platformio.ini` file and begin downloading the necessary toolchains and libraries (Frugal-IoT, DallasTemperature, etc.).
    * Wait for the "configuring project" tasks to finish in the bottom status bar.

### 3. Build Configuration
The project supports multiple hardware boards defined in `platformio.ini`. You must select the environment that matches your hardware.

**Primary Supported Board:**
* `nodemcu-32s` (Default for VARTA deployments)

**Other Environments:**
* `c3_pico`, `s2_mini`, `d1_mini`, `ttgo`, `t3s3`

### 4. How to Build & Upload

#### Option A: Using the VS Code Interface (Recommended)
1.  Click the **PlatformIO Alien Icon** on the left sidebar.
2.  In the **Project Tasks** menu, expand the folder for your board (e.g., `nodemcu-32s`).
3.  **Build**: Click **General > Build**. 
    * *PlatformIO will compile the firmware and create the binary in `.pio/build/nodemcu-32s/firmware.bin`.*
4.  **Upload**: Connect your board via USB and click **General > Upload**.
5.  **Monitor**: Click **General > Monitor** to see serial output (ensure baud rate is 460800).

#### Option B: Using the CLI Terminal
Open a terminal in VS Code (`Terminal > New Terminal`) and run:

```bash
# Build for the default NodeMCU-32s
pio run -e nodemcu-32s

# Build and Upload
pio run -e nodemcu-32s -t upload

# Build for other boards (e.g., ESP8266 D1 Mini)
pio run -e d1_mini
```

Once the board is flashed look for a WiFi network called something like esp32-1234, connect to this network. A captive portal should appear where you can select the WiFi and Password. It is possible to enter multiple WiFi's through either the captive Portal or as lines in main.cpp

### MQTT Configuration
The system connects to the default Frugal-IoT MQTT broker:
- **Host**: frugaliot.naturalinnovation.org
- **Username**: dev
- **Password**: public

### Sensor Calibration
Adjust soil moisture sensor calibration in `main.cpp`:
```cpp
// Parameters: id, name, pin, dry_value, scale_factor, color, retain
frugal_iot.sensors->add(new Sensor_Soil("soil", "Soil", 3, 4095, -100.0/4095, "brown", true));
```
This can also be calibrated in the captive portal. Place the soil moisture sensor in completely dry soil and set it as 0%, then adjust pout in water till the soil can absorb no more and set it to 100%.  
It is important to do it set the 0% value first, and it has to be 0% (as this is treated differently for calibration). 

### Power Management
Configure reading intervals and power modes:
```cpp
// Parameters: mode, cycle_ms, wake_ms
frugal_iot.configure_power(Power_Deep, 3600000, 20000); // 1 hour intervals
```

## Build Environments

Select the appropriate environment for your board:

```bash
# Node MCU 32
pio run -e nodemcu-32s

# ESP32-C3 Pico
pio run -e c3_pico

# ESP32-S2 Mini
pio run -e s2_mini

# ESP8266 D1 Mini
pio run -e d1_mini

# TTGO LoRa (with mesh support)
pio run -e ttgo

# LilyGo T3-S3 (with mesh support)
pio run -e t3s3
```

## Data Output

The system publishes sensor data via MQTT with the following topics structure:
- Soil moisture percentage
- Ambient temperature (°C)
- Ambient humidity (%)
- Soil temperature (°C)
- Battery voltage (V)

## Control Features

- **Hysteresis Control**: Automatic LED control based on soil moisture levels
- **Threshold**: 50% moisture with 1% hysteresis
- **Visual Feedback**: Built-in LED indicates system status

## Development

### Debug Flags
Enable debugging by uncommenting flags in `platformio.ini`:
```ini
-D SYSTEM_FRUGAL_DEBUG
-D SYSTEM_MQTT_DEBUG  
-D SENSOR_SOIL_DEBUG
```


## Troubleshooting

### Common Issues
1. **Sensor not detected**: Check wiring and I2C address
2. **WiFi connection fails**: Verify credentials and signal strength
3. **MQTT not connecting**: Check network connectivity and broker settings
4. **DS18B20 reading errors**: Ensure 4.7kΩ pullup resistor is connected

### Serial Monitor
Monitor output at 460800 baud for debugging information.

## Acknowledgement
This project is developed under a Grant from the [Gram Disha Trust (GDT)](https://gramdisha.org).  
Reference material, background discussions, and implementation notes are available on the [BLOG](https://gramdisha.org/iot-agritech-smallholders/).

## License

This project uses the Frugal-IoT framework. Please refer to the framework's license terms.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

For issues related to:
- **[Hardware setup](https://github.com/Tarun455/IoT-GDT/wiki/Hardware-Setup)**: Check wiring diagrams and pin configurations
- **Frugal-IoT framework**: Refer to the [official documentation](https://github.com/mitra42/frugal-iot/wiki)
- **PlatformIO**: Visit [PlatformIO documentation](https://docs.platformio.org/)
