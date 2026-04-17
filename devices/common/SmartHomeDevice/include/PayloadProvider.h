#pragma once

#include <ArduinoJson.h>


class PayloadProvider {
public:
    JsonDocument& getPayload(bool wrap = true);
protected:
    PayloadProvider(const char* fieldName) : payloadField(fieldName) {}

    virtual void build(JsonObject& payload) = 0;
private:
    JsonDocument payload;
    const char* payloadField;
};
