// Custom SHT sensor with validation for bad values
// Issue: https://github.com/Tarun455/VARTA_IoT-GDT/issues/20
//
// When both temperature AND humidity are zero simultaneously, the reading is invalid and should be rejected.

#include "Frugal-IoT.h"  // For Sensor_HT base class definition
#include "sensor_sht_validated.h"
#include <Wire.h>

/**
 * @brief Constructor
 * 
 * Sets up I2C communication for the SHT sensor.
 */
Sensor_SHT_Validated::Sensor_SHT_Validated(const char* const name, uint8_t address, TwoWire* wire, bool retain)
    : Sensor_HT("sht", name, retain),
      _address(address),
      _wire(wire) {
    // Setup I2C - defaults to system defined SDA and SCL
    _wire->begin(I2C_SDA, I2C_SCL);
    _wire->setClock(100000);
    _sht = new SENSOR_SHT_DEVICE_VALIDATED(_address, _wire);
}

/**
 * @brief Initializes the SHT sensor
 */
void Sensor_SHT_Validated::setup() {
    Sensor_HT::setup();
    _sht->begin();
    #ifdef SENSOR_SHT_DEBUG
        Serial.print(F("SHT Validated - address: ")); Serial.print(_address, HEX);
        #ifndef SENSOR_SHT_SHT4x
            Serial.print(F(" status: ")); Serial.print(_sht->readStatus(), HEX);
        #endif
        Serial.println();
    #endif
    _sht->requestData();  // Initial request queued up
}

/**
 * @brief Validates the temperature and humidity readings
 * 
 * Rejects readings where both temperature AND humidity are zero, as this indicates a sensor communication error.
 * 
 * @param temp Temperature value
 * @param humy Humidity value
 * @return bool True if the values are valid
 */
bool Sensor_SHT_Validated::validate(float temp, float humy) {
    // Reject if both temperature and humidity are zero simultaneously
    if (temp == 0.0f && humy == 0.0f) {
        #ifdef SENSOR_SHT_DEBUG
            Serial.println(F("SHT Validated: Rejecting invalid reading (both temp and humidity are zero)"));
        #endif
        return false;
    }
    return true;
}

/**
 * @brief Read, validate, convert and set sensor values
 * 
 * Reads temperature and humidity from the sensor, validates the readings, and only sets the values if they are valid.
 */
void Sensor_SHT_Validated::readValidateConvertSet() {
    #ifdef SENSOR_SHT_DEBUG
        Serial.print(_address, HEX);
        Serial.print(F("   "));
    #endif

    if (_sht->dataReady()) {
        if (_sht->readData()) {
            float temp = _sht->getTemperature();
            float humy = _sht->getHumidity();

            #ifdef SENSOR_SHT_DEBUG
                Serial.print(temp, 1);
                Serial.print(F("°C\t"));
                Serial.print(humy, 1);
                Serial.println(F("%"));
            #endif

            // Only set values if they pass validation
            if (validate(temp, humy)) {
                set(temp, humy);
            }

            // Request next reading
            _sht->requestData();

        #ifdef SENSOR_SHT_DEBUG
        } else {
            Serial.println(F("SHT sensor did not return data"));
        #endif
        }
    #ifdef SENSOR_SHT_DEBUG
    } else {
        Serial.println(F("SHT sensor not ready"));
    #endif
    }
}
