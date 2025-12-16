#ifndef SENSOR_INA219_H
#define SENSOR_INA219_H

// Forward declarations - breaks circular dependency main.cpp must include Frugal-IoT.h before this file
class Sensor;
class OUTfloat;

#include <Adafruit_INA219.h>

/**
 * INA219 Current/Voltage/Power Monitor Sensor
 * Provides three outputs: voltage, current, and battery percentage
 */
class Sensor_INA219 : public Sensor {
public:
    /**
     * Constructor
     * @param id Sensor identifier
     * @param name Display name
     * @param maxV Maximum voltage for percentage calculation (default: 4.2V)
     * @param minV Minimum voltage for percentage calculation (default: 3.0V)
     * @param retain Whether to retain MQTT messages
     */
    Sensor_INA219(const char* id, const char* name, float maxV = 4.2, float minV = 3.0, bool retain = true);

protected:
    // Override base class methods
    void setup() override;
    void readValidateConvertSet() override;

private:
    Adafruit_INA219 ina219;
    float maxVoltage;
    float minVoltage;
    bool initialized;
    
    // Output channels
    OUTfloat* out_voltage;
    OUTfloat* out_current;
    OUTfloat* out_percentage;
};

#endif // SENSOR_INA219_H
