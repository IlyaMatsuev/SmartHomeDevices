#pragma once


struct MqttSettings {
    const char* hostname;
    int port;
    const char* username;
    const char* password;

    MqttSettings()
        : hostname(nullptr), port(1883), username(nullptr), password(nullptr) {}

    MqttSettings(const char* hostname, const int port, const char* username, const char* password)
        : hostname(hostname), port(port), username(username), password(password) {}
};
