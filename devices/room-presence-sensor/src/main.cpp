#include <Arduino.h>

const uint8_t MOTION_SENSOR_PIN = 32;
const uint64_t MOTION_SENSOR_CALIBRATION_TIMEOUT_MS = 1000 * 30;
const uint64_t MOTION_COOLDOWN_MS = 1000 * 60 * 5;

const uint64_t SLEEP_DURATION_MS = 1000 * 60;


RTC_DATA_ATTR bool lastMotionDetected;
RTC_DATA_ATTR uint64_t motionHeldMs;

void sleep(uint64_t durationMs);

void tickMotionCooldown(uint64_t tickDuration) {
    motionHeldMs += tickDuration;
}

void resetMotionCooldown() {
    motionHeldMs = 0;
}

void setup() {
    Serial.begin(115200);
    pinMode(MOTION_SENSOR_PIN, INPUT);
    Serial.println();

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
        Serial.println("Calibrating PIR sensor...");
        lastMotionDetected = false;
        motionHeldMs = 0;
        delay(MOTION_SENSOR_CALIBRATION_TIMEOUT_MS);
    }
    Serial.println("Starting...");
}

void loop() {
    bool motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;
    bool pastMotionCooldown = MOTION_COOLDOWN_MS - motionHeldMs <= 0;

    if ((!lastMotionDetected && motionDetected) || pastMotionCooldown) {
        resetMotionCooldown();
        lastMotionDetected = motionDetected;
        Serial.printf("Motion %s\n", motionDetected ? "Detected" : "Not Detected");
    }

    Serial.printf("Sending information over MQTT (%llums since last change): %s\n", motionHeldMs, lastMotionDetected ? "Detected" : "Not Detected");

    tickMotionCooldown(SLEEP_DURATION_MS);
    sleep(SLEEP_DURATION_MS);
}

void sleep(uint64_t durationMs) {
    esp_sleep_enable_timer_wakeup(durationMs * 1000);
    if (!lastMotionDetected) {
        esp_sleep_enable_ext0_wakeup((gpio_num_t) MOTION_SENSOR_PIN, HIGH);
    }
    Serial.println("Going to sleep...");
    Serial.flush();
    esp_deep_sleep_start();
}
