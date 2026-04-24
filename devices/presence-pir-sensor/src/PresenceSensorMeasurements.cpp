#include "PresenceSensorMeasurements.h"


RTC_DATA_ATTR bool motionDetected;
RTC_DATA_ATTR uint64_t motionHeldMs;


PresenceSensorMeasurements::PresenceSensorMeasurements(const uint64_t motionColldownMs) {
    toggleIntervalUpdates(false);
    this->motionColldownMs = motionColldownMs;
    this->motionDetected = ::motionDetected;
    this->motionHeldMs = ::motionHeldMs;
}

void PresenceSensorMeasurements::tickMotionCooldown(const uint64_t timePassedMs) {
    this->motionHeldMs += timePassedMs;
    ::motionHeldMs = this->motionHeldMs;
}

void PresenceSensorMeasurements::setMotionDetection(bool detected) {
    bool pastMotionCooldown = this->motionHeldMs >= this->motionColldownMs;

    if ((!this->motionDetected && detected) || pastMotionCooldown) {
        this->motionDetected = detected;
        this->motionHeldMs = 0;
        Serial.printf("Motion %s\n", detected ? "Detected" : "Not Detected");
    }

    ::motionDetected = this->motionDetected;
    ::motionHeldMs = this->motionHeldMs;
}

bool PresenceSensorMeasurements::isMotionDetected() {
    return this->motionDetected;
}

void PresenceSensorMeasurements::build(JsonObject& measurements) {
    measurements["motion"] = this->motionDetected;
}
