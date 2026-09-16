#pragma once

#include "SmartHomeDevice.h"
#include "DoorLockControls.h"
#include "DoorLockMeasurements.h"

class DoorLockDevice : public SmartHomeDevice<DoorLockControls, DoorLockMeasurements> {
public:
    DoorLockDevice(const char* deviceName, const char* deviceType, uint64_t defaultOpenTimeoutMs) : SmartHomeDevice(
        deviceName,
        deviceType,
        new DoorLockControls(defaultOpenTimeoutMs),
        new DoorLockMeasurements(),
        NO_UPDATE_INTERVAL
    ) {}

    virtual void setup();
    virtual void loop() override;
};
