#include <Arduino.h>
#include "SmartHomeDevice.h"
#include "PresenceSensorControls.h"
#include "PresenceSensorMeasurements.h"
#include "PresenceSensorDevice.h"


const uint8_t MOTION_SENSOR_PIN = 32;
const uint64_t MOTION_SENSOR_CALIBRATION_TIMEOUT_MS = 1000 * 30;
const uint64_t MOTION_COOLDOWN_MS = 1000 * 60 * 2;

const uint64_t SLEEP_DURATION_MS = 1000 * 60;

const char* DEVICE_NAME = "presence-sensor";
const char* DEVICE_TYPE = "motion-sensor";


PresenceSensorDevice device(DEVICE_NAME, DEVICE_TYPE, MOTION_COOLDOWN_MS);

void setup() {
    Serial.begin(115200);
    device.setup((gpio_num_t) MOTION_SENSOR_PIN, MOTION_SENSOR_CALIBRATION_TIMEOUT_MS);
}

void loop() {
    bool motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;

    device.measurementsProvider->setMotionDetection(motionDetected);

    device.loop();
    device.sleep(SLEEP_DURATION_MS);
}
