#pragma once

#include "SmartHomeDevice.h"


class PresenceSensorMeasurements : public MeasurementsProvider {
public:
    PresenceSensorMeasurements(const uint64_t motionColldownMs);

    void setMotionDetection(bool detected);
    void tickMotionCooldown(const uint64_t timePassedMs);
    bool isMotionDetected();
protected:
    void build(JsonObject& measurements) override;
private:
    bool motionDetected;
    uint64_t motionHeldMs;
    uint64_t motionColldownMs;
};
