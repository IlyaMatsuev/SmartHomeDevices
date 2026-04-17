#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "NetworkSettings.h"
#include "MqttSettings.h"
#include "ControlsProvider.h"
#include "MeasurementsProvider.h"

// Increase if JSON messages become large than 1MB
const unsigned int MAX_MQTT_PACKET_SIZE = 1024;
const unsigned int DEFAULT_UPDATE_INTERVAL = 5000;
const unsigned int NO_UPDATE_INTERVAL = 0;


template<                                                                                                                                 
    typename TControls = ControlsProvider,
    typename TMeasurements = MeasurementsProvider                                                                                         
>
class SmartHomeDevice {
    static_assert(std::is_base_of<ControlsProvider, TControls>::value, "TControls must extend ControlsProvider");
    static_assert(std::is_base_of<MeasurementsProvider, TMeasurements>::value, "TMeasurements must extend MeasurementsProvider");

public:
    std::unique_ptr<TControls> controlsProvider;
    std::unique_ptr<TMeasurements> measurementsProvider;

    SmartHomeDevice(
        const char* deviceName,
        const char* deviceType,
        TControls* controlsProvider,
        TMeasurements* measurementsProvider,
        unsigned long updateIntervalMs = DEFAULT_UPDATE_INTERVAL
    ) :
        deviceName(deviceName),
        deviceType(deviceType),
        controlsProvider(std::unique_ptr<TControls>(controlsProvider)),
        measurementsProvider(std::unique_ptr<TMeasurements>(measurementsProvider)),
        updateIntervalMs(updateIntervalMs),
        mqttClient(wifiClient) {}

    virtual ~SmartHomeDevice() = default;

    virtual void setup(const NetworkSettings& networkSettings, const MqttSettings& mqttSettings);
    virtual void disableBluetooth();
    virtual void loop();
    virtual void sleep(uint64_t durationMs);

protected:
    const char* deviceName;
    const char* deviceType;
    unsigned long updateIntervalMs;

    bool isPaired() const;
    void sendMeasurementsUpdate();
    void sendControlsSync();

private:
    NetworkSettings networkSettings;
    MqttSettings mqttSettings;

    WiFiClient wifiClient;
    PubSubClient mqttClient;
    IPAddress deviceIp;
    String deviceId;

    String updateControlsTopic;
    String syncControlsTopic;
    String updateMeasurementsTopic;

    unsigned long lastPairRequest = 0;
    unsigned long lastUpdate = 0;

    void connectWiFi(const char* ssid, const char* password);
    void connectMqtt(const char* username, const char* password);
    void onMqttMessage(String topic, JsonObject data);
    void sendPairRequest();

    unsigned long msSinceLastPairRequest();
    unsigned long msSinceLastUpdate();

    String getUpdateControlsTopic();
    String getSyncControlsTopic();
    String getUpdateMeasurementsTopic();
};

#include "SmartHomeDevice.tpp"
