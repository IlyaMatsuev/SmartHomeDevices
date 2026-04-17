#pragma once

#include "SmartHomeDevice.h"


class PresenceSensorControls : public ControlsProvider {
public:
    PresenceSensorControls();

    void onUpdate(JsonObject& payload) override;
    bool isOn();
protected:
    void build(JsonObject& controls) override;
private:
    bool on;
};
