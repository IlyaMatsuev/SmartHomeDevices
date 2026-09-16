#include "DoorLockControls.h"


DoorLockControls::DoorLockControls(uint64_t defaultOpenTimeoutMs) {
    this->on = true;
    this->open = false;
    this->openTimeoutMs = defaultOpenTimeoutMs;
}

void DoorLockControls::onUpdate(JsonObject& payload) {
    if (payload["on"].is<bool>()) {
        this->on = payload["on"];
    }
    if (payload["open"].is<bool>()) {
        this->open = payload["open"];
    }
    if (payload["openTimeoutMs"].is<uint64_t>()) {
        this->openTimeoutMs = payload["openTimeoutMs"];
    }
}

bool DoorLockControls::isOn() {
    return this->on;
}

bool DoorLockControls::isOpen() {
    return this->open;
}

void DoorLockControls::build(JsonObject& controls) {
    controls["$override"] = true;
    controls["on"] = this->on;
    controls["open"] = this->open;
    controls["openTimeoutMs"] = this->openTimeoutMs;
}
