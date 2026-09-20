#include "ControlsProvider.h"

void ControlsProvider::onPair(JsonObject& payload) {
    this->onUpdate(payload);
}

void ControlsProvider::toggleControlsSync(bool sync) {
    this->controlsSynced = !sync;
}

bool ControlsProvider::controlsUpdated() {
    return this->controlsSynced;
}
