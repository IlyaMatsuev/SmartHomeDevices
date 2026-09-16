#include "secrets.h"
#include "DoorLockDevice.h"


void DoorLockDevice::setup() {
    Serial.println();
    disableBluetooth();

    // TODO: Assign necessary pins

    NetworkSettings networkSettings(NETWORK_SSID, NETWORK_PASSWORD);
    MqttSettings mqttSettings(MQTT_HOSTNAME, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
    SmartHomeDevice::setup(networkSettings, mqttSettings);
}

void DoorLockDevice::loop() {
    SmartHomeDevice::loop();
    delay(500);

    if (this->controlsProvider->isOn()) {
        if (this->controlsProvider->isUnlocked()) {
            // TODO: Open the gate for the lock
        } else {
            // TODO: Close the gate for the lock
        }
        
    }

    if (this->controlsProvider->isUnlocked() && this->controlsProvider->unnlockTimedOut()) {
        this->controlsProvider->lock();
    }
}
