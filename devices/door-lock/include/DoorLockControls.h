#pragma once

#include "SmartHomeDevice.h"

const uint64_t MIN_UNLOCK_TIMEOUT_MS = 1000;
const uint64_t MAX_UNLOCK_TIMEOUT_MS = 60000;

class DoorLockControls : public ControlsProvider {
public:
    DoorLockControls(uint64_t defaultUnlockTimeoutMs);

    void onUpdate(JsonObject& payload) override;
    void lock();
    bool isOn();
    bool isUnlocked();
    bool unnlockTimedOut();
protected:
    void build(JsonObject& controls) override;
private:
    bool on;
    bool unlocked;
    uint64_t unlockTimeoutMs;
    unsigned long lastUnlockedAt;
};
