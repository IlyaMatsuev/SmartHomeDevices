#include <Arduino.h>
#include "DoorLockDevice.h"

const char* DEVICE_NAME = "door-lock";
const char* DEVICE_TYPE = "lock";
const uint64_t DEFAULT_OPEN_TIMEOUT_MS = 5000;
const gpio_num_t LOCK_GATE_PIN = GPIO_NUM_6;

DoorLockDevice device(DEVICE_NAME, DEVICE_TYPE, DEFAULT_OPEN_TIMEOUT_MS);


void setup() {
    Serial.begin(115200);
    device.setup(LOCK_GATE_PIN);
}

void loop() {
    device.loop();
    delay(50);
}
