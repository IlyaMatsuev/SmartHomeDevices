#pragma once

#include "SmartHomeDevice.h"

class DoorLockControls : public ControlsProvider {
public:
    DoorLockControls(uint64_t defaultOpenTimeoutMs);

    void onUpdate(JsonObject& payload) override;
    bool isOn();
    bool isOpen();
protected:
    void build(JsonObject& controls) override;
private:
    bool on;
    bool open;
    uint64_t openTimeoutMs;
};
