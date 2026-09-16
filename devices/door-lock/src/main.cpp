#include <Arduino.h>
#include "DoorLockDevice.h"

const char* DEVICE_NAME = "door-lock";
const char* DEVICE_TYPE = "lock";
const uint64_t DEFAULT_OPEN_TIMEOUT_MS = 5000;

DoorLockDevice device(DEVICE_NAME, DEVICE_TYPE, DEFAULT_OPEN_TIMEOUT_MS);


void setup() {
    Serial.begin(115200);
    device.setup();
}

void loop() {
    device.loop();
    delay(500);
}
