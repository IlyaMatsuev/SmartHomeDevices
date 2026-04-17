#pragma once

#include "PayloadProvider.h"


class MeasurementsProvider : public PayloadProvider {
public:
    MeasurementsProvider() : PayloadProvider("measurements") {}

    void toggleIntervalUpdates(bool enable = true);
    bool intervalUpdatesEnabled();
private:
    bool enableIntervalUpdates = true;
};
