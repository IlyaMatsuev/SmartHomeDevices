#include "PresenceSensorControls.h"


PresenceSensorControls::PresenceSensorControls() {
    this->on = true;
}

void PresenceSensorControls::onUpdate(JsonObject& payload) {
    this->on = payload["on"];
}

void PresenceSensorControls::build(JsonObject& controls) {
    controls["$override"] = true;
    controls["on"] = this->on;
}
