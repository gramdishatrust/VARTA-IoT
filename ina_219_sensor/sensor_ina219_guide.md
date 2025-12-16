# INA219 Battery Monitor Sensor Guide

This guide explains how to add and use the INA219 current/voltage/power monitor sensor in your Frugal-IoT project for battery monitoring and power management.

## Overview

The INA219 is a high-side current/voltage monitor perfect for battery monitoring because:
- Measures bus voltage, shunt voltage, and power consumption
- High accuracy (±0.5% over temperature)
- I2C interface (only needs two pins: SDA and SCL)
- Multiple output channels (voltage, current, battery percentage)
- Wide voltage range (0-26V)
- Bi-directional current sensing

## Key Specifications

- **Voltage Range**: 0-26V (configurable for 16V or 32V)
- **Current Range**: Up to ±3.2A (configurable)
- **Interface**: I2C (address: 0x40 default)
- **Resolution**: 12-bit ADC
- **Operating Voltage**: 3.0-5.5V

## Hardware Setup

### Required Components
- INA219 current/voltage sensor module
- Battery or power source to monitor
- Jumper wires

### Wiring Diagram

```
INA219 Module     ESP32/ESP8266
-------------     -------------
VCC          →    3.3V
GND          →    GND
SDA          →    GPIO 21 (ESP32) / GPIO 4 (ESP8266)
SCL          →    GPIO 22 (ESP32) / GPIO 5 (ESP8266)

Power Monitoring:
VIN+         →    Battery Positive (+)
VIN-         →    Load/Circuit Positive (+)
                  (Measures current through shunt)
```

### Connection Details

The INA219 is inserted **in series** with your power supply to measure current:

```
Battery (+) → VIN+ → INA219 → VIN- → Load (+)
Battery (-) → Load (-)
```

**Important Notes:**
- The INA219 measures the voltage drop across an internal shunt resistor
- VIN+ connects to power source
- VIN- connects to your load
- GND is the common ground for both power and I2C communication

### I2C Address
Default address is `0x40`. Multiple sensors can be used by changing addresses via A0/A1 pins.

## Software Integration

### 1. Add Required Files
Copy these files to your `src/` directory:
- `sensor_ina219.h`
- `sensor_ina219.cpp`

### 2. Include Dependencies
The required library is already included in `platformio.ini`:
```ini
lib_deps = 
    adafruit/Adafruit INA219
    Frugal-IoT
```

### 3. Update main.cpp
Add the sensor to your Frugal-IoT setup:

```cpp
#include "Frugal-IoT.h"
#include "sensor_ina219.h"

System_Frugal frugal_iot("dev", "developers", "PowerMonitor", "Battery Monitoring System");

void setup() {
    frugal_iot.pre_setup();
    
    // Add other sensors...
    
    // Add INA219 battery monitor
    // Parameters: id, name, maxV, minV, retain
    frugal_iot.sensors->add(new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true));
    
    frugal_iot.setup();
}
```

## Sensor Parameters

When creating the sensor instance:
```cpp
new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true)
```

Parameters:
- `"battery"`: Unique sensor ID for MQTT topics
- `"Battery Monitor"`: Display name in UI
- `4.2`: Maximum voltage for percentage calculation (V) - default for LiPo
- `3.0`: Minimum voltage for percentage calculation (V) - default for LiPo
- `true`: Retain MQTT messages (recommended for power data)

### Battery Type Configurations

**LiPo Battery (Single Cell):**
```cpp
new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true)
```

**LiPo Battery (2S):**
```cpp
new Sensor_INA219("battery", "Battery Monitor", 8.4, 6.0, true)
```

**Lead Acid (12V):**
```cpp
new Sensor_INA219("battery", "Battery Monitor", 12.6, 10.5, true)
```

**USB Power (5V):**
```cpp
new Sensor_INA219("battery", "USB Power", 5.5, 4.5, true)
```

## MQTT Data Output

The sensor publishes **three separate outputs** to MQTT topics:

### 1. Voltage Output
- Topic: `{organization}/{project}/{device_id}/battery/voltage`
- Range: 0-32V
- Resolution: 2 decimal places
- Color: Yellow in dashboard
- Unit: Volts (V)

### 2. Current Output
- Topic: `{organization}/{project}/{device_id}/battery/current`
- Range: -3200 to +3200 mA
- Resolution: 1 decimal place
- Color: Orange in dashboard
- Unit: Milliamps (mA)
- Note: Negative values indicate charging (if applicable)

### 3. Battery Percentage Output
- Topic: `{organization}/{project}/{device_id}/battery/percentage`
- Range: 0-100%
- Resolution: 0 decimal places (integer)
- Color: Green in dashboard
- Unit: Percentage (%)
- Calculation: Linear interpolation between minV and maxV

## Advanced Configuration

### Calibration Ranges

The INA219 supports different calibration ranges. Uncomment one in `sensor_ina219.cpp`:

```cpp
void Sensor_INA219::setup() {
    if (ina219.begin()) {
        // Default: 32V, 2A range (auto-selected)
        
        // For lower current applications (better resolution):
        // ina219.setCalibration_32V_1A();
        
        // For very low power applications:
        // ina219.setCalibration_16V_400mA();
    }
}
```

**Calibration Options:**
- `32V_2A`: Default, best for most applications
- `32V_1A`: Better resolution for currents < 1A
- `16V_400mA`: Highest resolution for low-power sensors

### Enable Debug Output

Define the debug flag in `platformio.ini`:
```ini
build_flags = 
    -D SENSOR_INA219_DEBUG
```

This will print detailed readings to Serial Monitor (similar to this):
```
INA219 - Voltage: 3.85V, Current: 245.3mA, Battery: 56%
```

## Troubleshooting

### Common Issues

**Sensor not detected:**
- Check I2C wiring (SDA and SCL)
- Verify I2C address (default: 0x40)
- Ensure module is powered (VCC to 3.3V or 5V)
- Run I2C scanner to detect address

**Reading 0V or incorrect values:**
- Check VIN+ and VIN- connections
- Ensure power is flowing through the sensor
- Verify load is connected
- Check for reversed polarity

**Current always 0mA:**
- No current flowing (load not drawing power)
- Shunt resistor issue (rare, hardware fault)
- Wrong calibration range selected

**Battery percentage incorrect:**
- Verify maxV and minV parameters match your battery
- Use multimeter to verify actual voltage
- Adjust parameters for battery type

### I2C Scanner

To verify the sensor is detected, add this to `setup()`:
```cpp
#include <Wire.h>

void scanI2C() {
    Serial.println("Scanning I2C bus...");
    for (byte i = 8; i < 120; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.print("Found address: 0x");
            Serial.println(i, HEX);
        }
    }
}
```

Expected output: `Found address: 0x40`

### Serial Monitor Output
Check for initialization message:
```
INA219 initialized successfully
```

If you see:
```
Failed to find INA219 chip
```
Check wiring and I2C connections.

## Multiple Sensors

You can monitor multiple power sources using different I2C addresses:

```cpp
// Battery monitor on default address 0x40
frugal_iot.sensors->add(new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true));

// Solar panel on address 0x41 (configure A0 pin HIGH)
frugal_iot.sensors->add(new Sensor_INA219("solar", "Solar Panel", 6.0, 0.0, true));
```

**I2C Address Selection:**
- A0=LOW, A1=LOW: 0x40 (default)
- A0=HIGH, A1=LOW: 0x41
- A0=LOW, A1=HIGH: 0x44
- A0=HIGH, A1=HIGH: 0x45

## Integration with Other Sensors

Example complete agricultural monitoring with power tracking:

```cpp
#include "Frugal-IoT.h"
#include "sensor_ina219.h"

System_Frugal frugal_iot("dev", "developers", "AgriMonitor", "Agricultural Monitoring");

void setup() {
    frugal_iot.pre_setup();
    
    // Environmental sensors
    frugal_iot.sensors->add(new Sensor_SHT("SHT", SENSOR_SHT_ADDRESS, &I2C_WIRE, true));
    frugal_iot.sensors->add(new Sensor_Soil("soil", "Soil Moisture", 32, 4095, -100.0/4095, "brown", true));
    
    // Battery monitor (critical for solar-powered remote stations)
    frugal_iot.sensors->add(new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true));
    
    // Power management based on battery level
    frugal_iot.configure_power(Power_Light, 60000, 30000); // 1 min intervals
    
    frugal_iot.setup();
}
```

## Example Applications

### Solar Battery Monitor
```cpp
// LiPo battery charged by solar panel
frugal_iot.sensors->add(new Sensor_INA219("battery", "Solar Battery", 4.2, 3.2, true));
```
Positive current = discharging, Negative current = charging (if circuit supports)


### Multi-Battery System
```cpp
// Monitor multiple batteries with different addresses
frugal_iot.sensors->add(new Sensor_INA219("bat1", "Main Battery", 4.2, 3.0, true));
frugal_iot.sensors->add(new Sensor_INA219("bat2", "Backup Battery", 4.2, 3.0, true));
```

## Resources

- [Adafruit INA219 Guide](https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout)
- [INA219 Datasheet](https://www.ti.com/lit/ds/symlink/ina219.pdf)
