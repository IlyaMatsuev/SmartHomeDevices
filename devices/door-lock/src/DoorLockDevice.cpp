#include "secrets.h"
#include "DoorLockDevice.h"


void DoorLockDevice::setup(const gpio_num_t lockGatePin, const gpio_num_t statusLedPin) {
    this->lockGatePin = lockGatePin;
    this->statusLedPin = statusLedPin;

    // The relay input needs 5V for HIGH state (microcontroller's pin cannot give more than 3.3V),
    //  so invert the logic: HIGH -> locked, LOW -> unlocked
    digitalWrite(this->lockGatePin, HIGH);
    pinMode(this->lockGatePin, OUTPUT);

    signal(DeviceStatus::Booting);

    Serial.println();
    disableBluetooth();

    NetworkSettings networkSettings(NETWORK_SSID, NETWORK_PASSWORD);
    MqttSettings mqttSettings(MQTT_HOSTNAME, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
    SmartHomeDevice::setup(networkSettings, mqttSettings);
}

void DoorLockDevice::signal(const DeviceStatus& status) {
    LedColor color  = LedColor::of(status);
    bool colorOn = color.blinkIntervalMs == 0 || (millis() / color.blinkIntervalMs) % 2 == 0;
    uint8_t red = colorOn ? color.red : 0;
    uint8_t green = colorOn ? color.green : 0;
    uint8_t blue = colorOn ? color.blue : 0;

    neopixelWrite(this->statusLedPin, red, green, blue);
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

    updateStatus();
}

// TODO: Switch back to Connecting status if WiFi/MQTT suddenly stopped working
void DoorLockDevice::updateStatus() {
    if (!isPaired()) {
        signal(DeviceStatus::Connecting);
    } else if (this->controlsProvider->isUnlocked()) {
        signal(DeviceStatus::Unlocked);
    } else {
        signal(DeviceStatus::Locked);
    }
}
