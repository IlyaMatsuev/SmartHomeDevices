#pragma once

#include "SmartHomeDevice.h"
#include "PresenceSensorControls.h"
#include "PresenceSensorMeasurements.h"


class PresenceSensorDevice : public SmartHomeDevice<PresenceSensorControls, PresenceSensorMeasurements> {
public:
    PresenceSensorDevice(const char* deviceName, const char* deviceType, const uint64_t motionCooldownMs) : SmartHomeDevice(
        deviceName,
        deviceType,
        new PresenceSensorControls(),
        new PresenceSensorMeasurements(motionCooldownMs),
        NO_UPDATE_INTERVAL
    ) {}

    virtual void setup(const gpio_num_t motionSensorPin, const uint64_t calibrationTimeoutMs);
    virtual void loop() override;
    virtual void sleep(uint64_t durationMs) override;
private:
    gpio_num_t motionSensorPin;
};
