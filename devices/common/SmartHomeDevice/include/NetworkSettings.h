#pragma once


struct NetworkSettings {
    const char* wifiSsid;
    const char* wifiPassword;

    NetworkSettings()
        : wifiSsid(nullptr), wifiPassword(nullptr) {}

    NetworkSettings(const char* wifiSsid, const char* wifiPassword)
        : wifiSsid(wifiSsid), wifiPassword(wifiPassword) {}
};
