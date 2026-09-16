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
    if (this->controlsProvider->isOn() && this->controlsProvider->isOpen()) {
        // TODO: Open the gate for the lock
        // TODO: Switch the "open" toggle when the the openTimeoutMs is done
    }
    SmartHomeDevice::loop();
    delay(500);
}
