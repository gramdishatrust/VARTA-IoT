// Custom DS18B20 sensor with validation for bad values and full precision
// Issue: https://github.com/Tarun455/VARTA_IoT-GDT/issues/20
// 
// The DS18B20 sensor returns 85°C as its power-on reset value, which is invalid.
// This class overrides validate() to filter out these bad readings and
// returns full precision temperature values (no rounding).

#ifndef SENSOR_DS18B20_VALIDATED_H
#define SENSOR_DS18B20_VALIDATED_H

#include <OneWire.h>
#include <DallasTemperature.h>

// Forward declaration - Frugal-IoT.h must be included before this header in main.cpp
class Sensor_Float;

/**
 * @brief DS18B20 sensor with validation and full precision
 * 
 * Inherits from Sensor_Float and provides:
 * - Validation to reject NaN and values >= 80°C (power-on reset is 85°C)
 * - Full precision temperature readings (no rounding)
 */
class Sensor_DS18B20_Validated : public Sensor_Float {
public:
    /**
     * @brief Constructor for DS18B20 temperature sensor
     * 
     * @param id        Unique ID for the sensor
     * @param name      Human-readable sensor name
     * @param pin       GPIO pin connected to the DS18B20 data line
     * @param index     Sensor index on the OneWire bus (default = 0)
     * @param retain    Whether to retain last sensor value (true/false)
     */
    Sensor_DS18B20_Validated(const char* id, const char* name, uint8_t pin, uint8_t index, bool retain);

protected:
    /**
     * @brief Reads the current temperature in Celsius with full precision
     * @return float Temperature value or NAN if disconnected
     */
    float readFloat() override;

    /**
     * @brief Validates the temperature reading
     * Rejects NaN values and values >= 80°C (DS18B20 power-on reset is 85°C)
     * @param v The temperature value to validate
     * @return bool True if the value is valid
     */
    bool validate(float v) override;

    /**
     * @brief Initializes the sensor bus and prepares communication
     */
    void setup() override;

private:
    OneWire _oneWire;           // OneWire interface for DS18B20 communication
    DallasTemperature _sensors; // DallasTemperature driver instance
    uint8_t _index;             // Sensor index on the OneWire bus
};

#endif // SENSOR_DS18B20_VALIDATED_H
