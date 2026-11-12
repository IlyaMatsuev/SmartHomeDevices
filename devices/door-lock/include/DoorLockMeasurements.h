#pragma once

#include "SmartHomeDevice.h"

class DoorLockMeasurements : public MeasurementsProvider {
public:
    DoorLockMeasurements();

    void setUnlocked(bool unlocked);
protected:
    void build(JsonObject& measurements) override;
private:
    bool unlocked;
};
