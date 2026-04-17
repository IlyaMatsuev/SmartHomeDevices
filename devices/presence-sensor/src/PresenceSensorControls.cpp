#include "PresenceSensorControls.h"


PresenceSensorControls::PresenceSensorControls() {
    this->on = true;
}

void PresenceSensorControls::onUpdate(JsonObject& payload) {
    this->on = payload["on"];
}

bool PresenceSensorControls::isOn() {
    return this->on;
}

void PresenceSensorControls::build(JsonObject& controls) {
    controls["$override"] = true;
    controls["on"] = this->on;
}
