#include "DoorLockControls.h"


DoorLockControls::DoorLockControls(uint64_t defaultUnlockTimeoutMs) {
    this->on = true;
    this->unlocked = false;
    this->unlockTimeoutMs = defaultUnlockTimeoutMs;
    this->lastUnlockedAt = 0;
}

void DoorLockControls::onUpdate(JsonObject& payload) {
    if (payload["on"].is<bool>()) {
        this->on = payload["on"];
    }
    if (payload["unlocked"].is<bool>()) {
        this->unlocked = payload["unlocked"];
        if (this->unlocked) {
            this->lastUnlockedAt = millis();
        }
    }
    if (payload["unlockTimeoutMs"].is<uint64_t>()) {
        uint64_t unlockTimeoutMs = payload["unlockTimeoutMs"].as<uint64_t>();
        unlockTimeoutMs = unlockTimeoutMs < MIN_UNLOCK_TIMEOUT_MS ? MIN_UNLOCK_TIMEOUT_MS : unlockTimeoutMs;
        unlockTimeoutMs = unlockTimeoutMs > MAX_UNLOCK_TIMEOUT_MS ? MAX_UNLOCK_TIMEOUT_MS : unlockTimeoutMs;
        this->unlockTimeoutMs = unlockTimeoutMs;
    }
}

void DoorLockControls::onPair(JsonObject& payload) {
    ControlsProvider::onPair(payload);
    // When pairing with a hub, the door must always be initially locked, no matter the state on the hub
    lock();
}

void DoorLockControls::lock() {
    this->unlocked = false;
    this->lastUnlockedAt = 0;
    this->toggleControlsSync(true);
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
    return msSinceUnlocked >= this->unlockTimeoutMs;
}

void DoorLockControls::build(JsonObject& controls) {
    controls["$override"] = true;
    controls["on"] = this->on;
    controls["unlocked"] = this->unlocked;
    controls["unlockTimeoutMs"] = this->unlockTimeoutMs;
}
