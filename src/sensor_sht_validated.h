// Custom SHT sensor with validation for bad values
// Issue: https://github.com/Tarun455/VARTA_IoT-GDT/issues/20
// 
// When both temperature AND humidity are zero simultaneously,
// the reading is invalid and should be rejected.

#ifndef SENSOR_SHT_VALIDATED_H
#define SENSOR_SHT_VALIDATED_H

// Forward declarations - Frugal-IoT.h must be included before this header in main.cpp
class Sensor_HT;
class TwoWire;

#ifdef SENSOR_SHT_SHT4x
  #include <SHT4x.h>
#else
  #include <SHT85.h>
#endif

// Define the device type
#ifdef SENSOR_SHT_SHT4x
  #define SENSOR_SHT_DEVICE_VALIDATED SHT4x
#else
  #define SENSOR_SHT_DEVICE_VALIDATED SHT30
#endif

/**
 * @brief SHT sensor with validation to reject bad readings
 * 
 * Inherits from Sensor_HT and overrides readValidateConvertSet() to reject:
 * - Readings where both temperature AND humidity are zero simultaneously
 */
class Sensor_SHT_Validated : public Sensor_HT {
public:
    /**
     * @brief Constructor for SHT temperature/humidity sensor
     * 
     * @param name      Human-readable sensor name
     * @param address   I2C address (0x44 or 0x45)
     * @param wire      TwoWire interface for I2C
     * @param retain    Whether to retain last sensor value
     */
    Sensor_SHT_Validated(const char* const name, uint8_t address, TwoWire* wire, bool retain);

protected:
    /**
     * @brief Read, validate, convert and set sensor values
     * Rejects readings where both temperature and humidity are zero
     */
    void readValidateConvertSet() override;

    /**
     * @brief Initializes the sensor
     */
    void setup() override;

    /**
     * @brief Validates the temperature and humidity readings
     * @param temp Temperature value
     * @param humy Humidity value
     * @return bool True if the values are valid (not both zero)
     */
    bool validate(float temp, float humy);

private:
    uint8_t _address;
    SENSOR_SHT_DEVICE_VALIDATED* _sht;
    TwoWire* _wire;
};

#endif // SENSOR_SHT_VALIDATED_H
