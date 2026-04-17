#include "ControlsProvider.h"


void ControlsProvider::toggleControlsSync(bool sync) {
    this->controlsSynced = !sync;
}

bool ControlsProvider::controlsUpdated() {
    return this->controlsSynced;
}
