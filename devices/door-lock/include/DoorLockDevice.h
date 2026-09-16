#pragma once

#include "SmartHomeDevice.h"
#include "DoorLockControls.h"
#include "DoorLockMeasurements.h"

class DoorLockDevice : public SmartHomeDevice<DoorLockControls, DoorLockMeasurements> {
public:
    DoorLockDevice(const char* deviceName, const char* deviceType, uint64_t defaultUnlockTimeoutMs) : SmartHomeDevice(
        deviceName,
        deviceType,
        new DoorLockControls(defaultUnlockTimeoutMs),
        new DoorLockMeasurements(),
        NO_UPDATE_INTERVAL
    ) {}

    virtual void setup();
    virtual void loop() override;
};
