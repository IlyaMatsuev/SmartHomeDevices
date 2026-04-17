#include "MeasurementsProvider.h"


void MeasurementsProvider::toggleIntervalUpdates(bool enable) {
    this->enableIntervalUpdates = enable;
}

bool MeasurementsProvider::intervalUpdatesEnabled() {
    return this->enableIntervalUpdates;
}
