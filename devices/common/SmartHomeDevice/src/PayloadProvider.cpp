#include "PayloadProvider.h"


JsonDocument& PayloadProvider::getPayload(bool wrap) {
    payload.clear();

    JsonObject root = payload.to<JsonObject>();
    if (wrap) {
        JsonObject values = root[payloadField].to<JsonObject>();
        build(values);
    } else {
        build(root);
    }
    return payload;
}
