#include <Arduino.h>

const uint8_t MOTION_SENSOR_PIN = 33;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting...");
}

void loop() {
    bool motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;
    if (motionDetected) {
        Serial.printf("motion Detected: %d \n", motionDetected);
    } else {
        Serial.printf("motion Not Detected: %d \n", motionDetected);
    }
    delay(500);
}
