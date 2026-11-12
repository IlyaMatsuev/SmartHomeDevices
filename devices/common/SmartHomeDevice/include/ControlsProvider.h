#pragma once

#include "PayloadProvider.h"


class ControlsProvider : public PayloadProvider {
public:
    ControlsProvider() : PayloadProvider("controls") {}

    // TODO: Try to create a way to provide a type for this payload parameter
    virtual void onUpdate(JsonObject& payload) = 0;
};
