#include "secrets.h"
#include "DoorLockDevice.h"


void DoorLockDevice::setup(const gpio_num_t lockGatePin, const gpio_num_t statusLedPin) {
    this->lockGatePin = lockGatePin;
    this->statusLedPin = statusLedPin;

    this->updateLockGatePin(false);
    pinMode(this->lockGatePin, OUTPUT);

    signal(DeviceStatus::Booting);

    Serial.println();
    disableBluetooth();

    NetworkSettings networkSettings(NETWORK_SSID, NETWORK_PASSWORD);
    MqttSettings mqttSettings(MQTT_HOSTNAME, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
    SmartHomeDevice::setup(networkSettings, mqttSettings);
}

void DoorLockDevice::loop() {
    SmartHomeDevice::loop();
    delay(50);

    updateLockState();
    updateStatus();
}

void DoorLockDevice::onPair(JsonObject& payload) {
    SmartHomeDevice::onPair(payload);
    // When pairing with a hub, the door must always be initially locked, no matter the state on the hub
    this->controlsProvider->lock();
    // Report the initial measurements state
    this->measurementsProvider->requestSendUpdate();
}

void DoorLockDevice::onConnecting() {
    SmartHomeDevice::onConnecting();
    signal(DeviceStatus::Connecting);

    // Lock the door when connection with WiFi/MQTT is lost
    if (this->controlsProvider->isUnlocked()) {
        updateLockState();
    }
}

void DoorLockDevice::signal(const DeviceStatus& status) {
    LedColor color  = LedColor::of(status);
    bool colorOn = color.blinkIntervalMs == 0 || (millis() / color.blinkIntervalMs) % 2 == 0;
    uint8_t red = colorOn ? color.red : 0;
    uint8_t green = colorOn ? color.green : 0;
    uint8_t blue = colorOn ? color.blue : 0;

    neopixelWrite(this->statusLedPin, red, green, blue);
}

void DoorLockDevice::updateLockState() {
    // Lock on timeout even if the device is "off"
    if (this->controlsProvider->isUnlocked() && (this->controlsProvider->unlockTimedOut() || !this->controlsProvider->isOn())) {
        Serial.printf("Timeout is done, locking\n");
        this->controlsProvider->lock();
    }

    this->updateLockGatePin(this->controlsProvider->isOn() && this->controlsProvider->isUnlocked());

    this->measurementsProvider->setUnlocked(this->controlsProvider->isUnlocked());
}

void DoorLockDevice::updateLockGatePin(bool unlocked) {
    // Relay coil is switched by a MOSSFET. HIGH -> unlocked, LOW -> locked
    digitalWrite(this->lockGatePin, unlocked ? HIGH : LOW);
}

void DoorLockDevice::updateStatus() {
    if (!isPaired()) {
        signal(DeviceStatus::Connecting);
    } else if (this->controlsProvider->isUnlocked()) {
        signal(DeviceStatus::Unlocked);
    } else {
        signal(DeviceStatus::Locked);
    }
}
