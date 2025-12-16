#include "Frugal-IoT.h"
#include "sensor_ina219.h"

Sensor_INA219::Sensor_INA219(const char* id, const char* name, float maxV, float minV, bool retain)
    : Sensor(id, name, retain), 
      maxVoltage(maxV), 
      minVoltage(minV), 
      initialized(false) {
    
    // Create three output channels for this sensor
    // Format: OUTfloat(sensorId, id, name, value, decimals, min, max, color, wireable)
    out_voltage = new OUTfloat(id, "voltage", "Voltage", 0.0, 2, 0.0, 32.0, "yellow", true);
    outputs.push_back(out_voltage);
    
    out_current = new OUTfloat(id, "current", "Current", 0.0, 1, -3200.0, 3200.0, "orange", true);
    outputs.push_back(out_current);
    
    out_percentage = new OUTfloat(id, "percentage", "Battery", 0.0, 0, 0.0, 100.0, "green", true);
    outputs.push_back(out_percentage);
}

void Sensor_INA219::setup() {
    // Initialize the INA219 sensor
    if (!ina219.begin()) {
        Serial.println(F("Failed to find INA219 chip"));
        initialized = false;
    } else {
        Serial.println(F("INA219 initialized successfully"));
        initialized = true;
        // Default calibration for 32V, 2A range
        // Uncomment one of these for different ranges:
        // ina219.setCalibration_32V_1A();
        // ina219.setCalibration_16V_400mA();
    }
}

void Sensor_INA219::readValidateConvertSet() {
    if (!initialized) {
        return;
    }

    // Read voltage from INA219
    float voltage = ina219.getBusVoltage_V();
    out_voltage->set(voltage);

    // Read current from INA219
    float current = ina219.getCurrent_mA();
    out_current->set(current);

    // Calculate battery percentage (linear approximation between min and max voltage)
    float percentage = ((voltage - minVoltage) / (maxVoltage - minVoltage)) * 100.0;
    percentage = constrain(percentage, 0.0, 100.0);
    out_percentage->set(percentage);

#ifdef SENSOR_INA219_DEBUG
    Serial.print(F("INA219 - Voltage: "));
    Serial.print(voltage);
    Serial.print(F("V, Current: "));
    Serial.print(current);
    Serial.print(F("mA, Battery: "));
    Serial.print(percentage);
    Serial.println(F("%"));
#endif
}
