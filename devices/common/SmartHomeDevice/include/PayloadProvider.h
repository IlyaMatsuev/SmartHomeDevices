#pragma once

#include <ArduinoJson.h>


class PayloadProvider {
public:
    JsonDocument& getPayload(bool wrap = true);

    // Toggle to send the payload in the next loop (on demand, without interval)
    void requestSendUpdate(bool request = true);
    bool isPendingUpdate();
protected:
    PayloadProvider(const char* fieldName) : payloadField(fieldName) {}

    virtual void build(JsonObject& payload) = 0;
private:
    JsonDocument payload;
    const char* payloadField;
    bool pendingUpdate = false;
};
