#pragma once

#include "SmartHomeDevice.h"
#include "DoorLockControls.h"
#include "DoorLockMeasurements.h"


enum class DeviceStatus {
    Booting,
    Connecting,
    Locked,
    Unlocked,
};

struct LedColor {
    uint8_t red, green, blue;
    uint64_t blinkIntervalMs;

    static LedColor of(DeviceStatus status) {
        switch (status)
        {
        case DeviceStatus::Booting:
            // Orange, no blink interval
            return { 40, 20, 0, 0 };
        case DeviceStatus::Connecting:
            // Yellow, 200ms blink interval
            return { 40, 40, 0, 200 };
        case DeviceStatus::Locked:
            // Red, no blink interval
            return { 40, 0, 0, 0 };
        case DeviceStatus::Unlocked:
            // Green, no blink interval
            return { 0, 40, 0, 0 };
        }
        return { 0, 0, 0, 0 };
    }
};


class DoorLockDevice : public SmartHomeDevice<DoorLockControls, DoorLockMeasurements> {
public:
    DoorLockDevice(const char* deviceName, const char* deviceType, uint64_t defaultUnlockTimeout) : SmartHomeDevice(
        deviceName,
        deviceType,
        new DoorLockControls(defaultUnlockTimeout),
        new DoorLockMeasurements(),
        NO_UPDATE_INTERVAL
    ) {}

    virtual void setup(const gpio_num_t lockGatePin, const gpio_num_t statusLedPin);
    virtual void loop() override;
protected:
    void signal(const DeviceStatus& status);
    void updateLockState();

    void onPair(JsonObject& payload) override;
    void onConnecting() override;
private:
    gpio_num_t lockGatePin;
    gpio_num_t statusLedPin;

    void updateStatus();
};
