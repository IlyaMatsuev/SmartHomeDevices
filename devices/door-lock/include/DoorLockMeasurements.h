#pragma once

#include "SmartHomeDevice.h"

class DoorLockMeasurements : public MeasurementsProvider {
public:
    DoorLockMeasurements();
protected:
    void build(JsonObject& measurements) override;
};
