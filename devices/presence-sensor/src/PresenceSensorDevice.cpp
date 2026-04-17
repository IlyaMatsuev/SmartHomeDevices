#include "secrets.h"
#include "PresenceSensorDevice.h"


void PresenceSensorDevice::setup(const gpio_num_t motionSensorPin, const uint64_t calibrationTimeoutMs) {
    Serial.println();

    this->motionSensorPin = motionSensorPin;
    pinMode(motionSensorPin, INPUT);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
        Serial.println("Calibrating PIR sensor...");
        delay(calibrationTimeoutMs);
    }

    NetworkSettings networkSettings(NETWORK_SSID, NETWORK_PASSWORD);
    MqttSettings mqttSettings(MQTT_HOSTNAME, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
    SmartHomeDevice::setup(networkSettings, mqttSettings);
}

void PresenceSensorDevice::loop() {
    while (!isPaired()) {
        SmartHomeDevice::loop();
    }

    sendControlsSync();
    sendMeasurementsUpdate();
}

void PresenceSensorDevice::sleep(uint64_t durationMs) {
    // Tick motion detection cooldown by the same duration as device is going to be asleep
    this->measurementsProvider->tickMotionCooldown(durationMs);

    if (!this->measurementsProvider->isMotionDetected()) {
        Serial.println("Preparing to wake up on motion detection");
        esp_sleep_enable_ext0_wakeup(this->motionSensorPin, HIGH);
    }
    SmartHomeDevice::sleep(durationMs);
}