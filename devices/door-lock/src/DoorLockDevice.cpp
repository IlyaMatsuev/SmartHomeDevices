#include "secrets.h"
#include "DoorLockDevice.h"


void DoorLockDevice::setup(const gpio_num_t lockGatePin) {
    this->lockGatePin = lockGatePin;
    // The relay input needs more than 3.3V for HIGH state, so invert the logic (HIGH for "locked" state)
    digitalWrite(this->lockGatePin, HIGH);
    pinMode(this->lockGatePin, OUTPUT);

    Serial.println();
    disableBluetooth();

    NetworkSettings networkSettings(NETWORK_SSID, NETWORK_PASSWORD);
    MqttSettings mqttSettings(MQTT_HOSTNAME, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
    SmartHomeDevice::setup(networkSettings, mqttSettings);
}

void DoorLockDevice::loop() {
    // TODO: If it was unlocked by the update from hub in the previous iteration,
    //  and then WIFI/MQTT connection dropped,
    //  the pin would stay unlocked until the device is connected to Wifi/MQTT or the device loses power
    SmartHomeDevice::loop();
    delay(50);

    if (this->controlsProvider->isUnlocked() && this->controlsProvider->unlockTimedOut()) {
        Serial.printf("Timeout is done, locking\n");
        this->controlsProvider->lock();
    }

    digitalWrite(this->lockGatePin, !(this->controlsProvider->isOn() && this->controlsProvider->isUnlocked()) ? HIGH : LOW);
}
