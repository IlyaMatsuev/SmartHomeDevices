#include "DoorLockControls.h"


DoorLockControls::DoorLockControls(uint64_t defaultUnlockTimeout) {
    this->on = true;
    this->unlocked = false;
    this->unlockTimeout = defaultUnlockTimeout;
    this->lastUnlockedAt = 0;
}

void DoorLockControls::onUpdate(JsonObject& payload) {
    if (payload["on"].is<bool>()) {
        this->on = payload["on"];
    }
    if (payload["unlockTimeout"].is<uint64_t>()) {
        uint64_t unlockTimeout = payload["unlockTimeout"].as<uint64_t>();
        unlockTimeout = unlockTimeout < MIN_UNLOCK_TIMEOUT_MS ? MIN_UNLOCK_TIMEOUT_MS : unlockTimeout;
        unlockTimeout = unlockTimeout > MAX_UNLOCK_TIMEOUT_MS ? MAX_UNLOCK_TIMEOUT_MS : unlockTimeout;
        this->unlockTimeout = unlockTimeout;
    }

    if (payload["unlock"].is<bool>() && payload["unlock"] == true && isOn()) {
        this->unlocked = payload["unlock"];
        if (this->unlocked) {
            this->lastUnlockedAt = millis();
        }
    }
}

void DoorLockControls::lock() {
    this->unlocked = false;
    this->lastUnlockedAt = 0;
}

bool DoorLockControls::isOn() {
    return this->on;
}

bool DoorLockControls::isUnlocked() {
    return this->unlocked;
}

bool DoorLockControls::unlockTimedOut() {
    if (this->lastUnlockedAt == 0) {
        return true;
    }
    unsigned long msSinceUnlocked = millis() - this->lastUnlockedAt;
    return msSinceUnlocked >= this->unlockTimeout;
}

void DoorLockControls::build(JsonObject& controls) {
    controls["$override"] = true;
    controls["on"] = this->on;
    controls["unlockTimeout"] = this->unlockTimeout;
}
