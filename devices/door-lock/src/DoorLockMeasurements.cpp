#include "DoorLockMeasurements.h"

DoorLockMeasurements::DoorLockMeasurements() {
    toggleIntervalUpdates(false);
    this->unlocked = false;
}

void DoorLockMeasurements::setUnlocked(bool unlocked) {
    if (this->unlocked != unlocked) {
        this->unlocked = unlocked;
        requestSendUpdate();
    }
}

void DoorLockMeasurements::build(JsonObject& measurements) {
    measurements["unlocked"] = this->unlocked;
}
