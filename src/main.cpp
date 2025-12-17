/* 
 *  Frugal IoT example - SOIL Sensor, with SHT and Voltage Divider Battery Monitor
 * 
 * Optional: SENSOR_SHT_ADDRESS - defaults to 0x44, (note the D1 shields default to 0x45)
 */

// #ifndef SYSTEM_POWER_CYCLE
//   #define SYSTEM_POWER_CYCLE 900000 // 15 minutes total cycle (in ms)
// #endif
// #ifndef SYSTEM_POWER_WAKE
//   #define SYSTEM_POWER_WAKE 30000 // Wake for 30 seconds and take readings
// #endif


// defines SENSOR_SHT_ADDRESS if dont define here or in platformio.ini
#include "Frugal-IoT.h"

// Change the parameters here to match your ... 
// organization, project, id, description
System_Frugal frugal_iot("varta", "developers", "Agriculture", "Agriculture Sensor"); 

void setup() {
  frugal_iot.pre_setup(); // Encapsulate setting up and starting serial and read main config

  // Override MQTT host, username and password if you have an "organization" other than "dev" (developers)
  frugal_iot.configure_mqtt("frugaliot.naturalinnovation.org", "varta", "notverysecret");

  // Configure power handling - type, cycle_ms, wake_ms 
  // power will be awake wake_ms then for the rest of cycle_ms be in a mode defined by type 
  // Loop= awake all the time; 
  // Light = Light Sleep; 
  // LightWiFi=Light + WiFi on (not working); 
  // Modem=Modem sleep - works but negligable power saving
  // Deep - works but slow recovery and slow response to UX so do not use except for multi minute cycles.
  frugal_iot.configure_power(Power_Loop, 30000, 30000); // Take a reading every 30 seconds - awake all the time
  // frugal_iot.configure_power(Power_Deep, SYSTEM_POWER_CYCLE, SYSTEM_POWER_WAKE); // Wake for 30 seconds, sleep for 15 minutes cycle
  // system_oled and actuator_ledbuiltin added automatically on boards that have them.

  // Add local wifis here, or see instructions in the wiki for adding via the /data
  //frugal_iot.wifi->addWiFi(F("Thakur"),F("thakur123"));
  
  // Add sensors, actuators and controls (using validated SHT to filter invalid readings)
  frugal_iot.sensors->add(new Sensor_SHT("SHT", SENSOR_SHT_ADDRESS, &I2C_WIRE, true));

  // Soil sensor 0%=4095 100%=0 pin=3 smooth=0 color=brown
  frugal_iot.sensors->add(new Sensor_Soil("soil", "Soil", 34, 4095, -100.0/4095, "brown", true));
  
  ControlHysterisis* cb = new ControlHysterisis("controlhysterisis", "Control", 50, 1, 0, 100);
  frugal_iot.controls->add(cb);
  cb->outputs[0]->wireTo(frugal_iot.messages->path("ledbuiltin/on"));

  // DS18B20 sensor for soil temperature (using validated subclass to filter bad 85°C values)
  frugal_iot.sensors->add(new Sensor_DS18B20("ds18b20", "Soil Temperature", 5, 0, true));

  frugal_iot.sensors->add(new Sensor_Battery(32,2.0));

  // INA219 battery monitor - maxV=4.2V, minV=3.0V for LiPo
  // frugal_iot.sensors->add(new Sensor_INA219("battery", "Battery Monitor", 4.2, 3.0, true));

  // Dont change below here - should be after setup the actuators, controls and sensors
  frugal_iot.setup(); // Has to be after setup sensors and actuators and controls and sysetm
  Serial.println(F("FrugalIoT Starting Loop"));
}

void loop() {
  frugal_iot.loop(); // Should be running watchdog.loop which will call esp_task_wdt_reset()
}

