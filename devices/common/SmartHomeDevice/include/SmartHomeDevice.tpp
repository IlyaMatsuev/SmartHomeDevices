#include <esp_bt.h>

#include "SmartHomeDevice.h"


static const int MQTT_CONNECT_RETRY_DELAY = 5000;
static const int PAIR_REQUEST_INTERVAL = 5000;

static const char* PAIR_REQUEST_TOPIC = "home/devices/pair";
static const char* PAIR_REQUEST_REPLY_TOPIC = "home/devices/pair/reply";
static const char* UPDATE_CONTROLS_TOPIC = "home/devices/%s/controls/update";
static const char* SYNC_CONTROLS_TOPIC = "home/devices/%s/controls/sync";
static const char* UPDATE_MEASUREMENTS_TOPIC = "home/devices/%s/measurements/update";


template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::setup(const NetworkSettings& networkSettings, const MqttSettings& mqttSettings) {
    this->networkSettings = networkSettings;
    this->mqttSettings = mqttSettings;

    connectWiFi(this->networkSettings.wifiSsid, this->networkSettings.wifiPassword);

    mqttClient.setServer(this->mqttSettings.hostname, this->mqttSettings.port);
    mqttClient.setCallback([this](char* topic, byte* messageBytes, unsigned int length) {
        Serial.printf("\nMessage received [%s]: %.*s\n\n", topic, length, messageBytes);

        JsonDocument data;
        DeserializationError error = deserializeJson(data, messageBytes, length);
        if (error) {
            Serial.printf("Failed to deserialize JSON: %s\n", error.c_str());
        } else {
            this->onMqttMessage(String(topic), data["data"].as<JsonObject>());
        }
    });
    connectMqtt(this->mqttSettings.username, this->mqttSettings.password);
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::disableBluetooth() {
    esp_bt_controller_disable();
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::loop() {
    if (!WiFi.isConnected()) {
        connectWiFi(this->networkSettings.wifiSsid, this->networkSettings.wifiPassword);
    }
    if (!mqttClient.connected()) {
        connectMqtt(this->mqttSettings.username, this->mqttSettings.password);
    }
    mqttClient.loop();

    Serial.printf("Device id: %s, ms since last pair: %d, ms since last update: %d\n", deviceId.c_str(), msSinceLastPairRequest(), msSinceLastUpdate());
    if (!isPaired() && msSinceLastPairRequest() >= PAIR_REQUEST_INTERVAL) {
        sendPairRequest();
        lastPairRequest = millis();
    }

    if (isPaired() && measurementsProvider->intervalUpdatesEnabled() && msSinceLastUpdate() >= updateIntervalMs) {
        sendMeasurementsUpdate();
        lastUpdate = millis();
    }

    if (isPaired() && !controlsProvider->controlsUpdated()) {
        sendControlsSync();
        controlsProvider->toggleControlsSync(false);
    }
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::sleep(uint64_t durationMs) {
    // Delay to send all MQTT messages before disconnecting
    delay(500);

    if (mqttClient.connected()) {
        mqttClient.disconnect();
    }
    if (WiFi.isConnected()) {
        WiFi.disconnect(true);
    }

    if (durationMs > 0) {
        Serial.printf("Preparing to sleep for %llu ms\n", durationMs);
        esp_sleep_enable_timer_wakeup(durationMs * 1000);
    }
    Serial.println("Going to sleep...");
    Serial.flush();
    esp_deep_sleep_start();
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::connectWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (!WiFi.isConnected()) {
        delay(100);
        Serial.print(".");
    }
    deviceIp = WiFi.localIP();
    Serial.println("\nConnected to Wi-Fi, IP: " + deviceIp.toString());
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::connectMqtt(const char* username, const char* password) {
    Serial.printf("\nConnecting to mqtt://%s:%d as %s\n", this->mqttSettings.hostname, this->mqttSettings.port, deviceName);
    while (!mqttClient.connected()) {
        if (mqttClient.connect(deviceName, username, password)) {
            mqttClient.setBufferSize(MAX_MQTT_PACKET_SIZE);
            mqttClient.subscribe(PAIR_REQUEST_REPLY_TOPIC);
            sendPairRequest();
        } else {
            Serial.printf("Failed to connect: %d\n, trying again in %d ms", mqttClient.state(), MQTT_CONNECT_RETRY_DELAY);
            delay(MQTT_CONNECT_RETRY_DELAY);
        }
    }
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::onMqttMessage(String topic, JsonObject data) {
    if (topic == PAIR_REQUEST_REPLY_TOPIC) {
        // { "accepted": true, "deviceId": "xxx", "updateInterval": 10000, "message": "example" }
        if (data["accepted"]) {
            deviceId = data["deviceId"].as<String>();
            updateIntervalMs = data["updateInterval"];

            JsonObject deviceControls = data["controls"].as<JsonObject>();
            this->controlsProvider->onUpdate(deviceControls);

            Serial.printf("Received pairing acceptance (%s), with update interval: %d\n", deviceId.c_str(), updateIntervalMs);

            mqttClient.subscribe(getUpdateControlsTopic().c_str());
            mqttClient.unsubscribe(PAIR_REQUEST_REPLY_TOPIC);
        } else {
            Serial.printf("Pairing got rejected with message: %s\n", data["message"].as<String>().c_str());
        }
    }
    if (topic == getUpdateControlsTopic()) {
        // { ...controls } - Can contain any kind of controls
        Serial.println("Received controls update request");
        this->controlsProvider->onUpdate(data);
    }
}

template<typename TControls, typename TMeasurements>
bool SmartHomeDevice<TControls, TMeasurements>::isPaired() const {
    return deviceId.length() > 0;
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::sendPairRequest() {
    JsonDocument request;
    request["deviceIp"] = deviceIp.toString();
    request["deviceName"] = deviceName;
    request["deviceType"] = deviceType;
    request["updateInterval"] = updateIntervalMs;
    // TODO: getPayload() always returns null
    request["controls"] = controlsProvider->getPayload(false);
    request["measurements"] = measurementsProvider->getPayload(false);

    char requestJson[MAX_MQTT_PACKET_SIZE];
    serializeJson(request, requestJson);
    Serial.printf("\nSending pairing request: %s\n", requestJson);

    mqttClient.publish(PAIR_REQUEST_TOPIC, requestJson);
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::sendMeasurementsUpdate() {
    char requestJson[MAX_MQTT_PACKET_SIZE];
    serializeJson(measurementsProvider->getPayload(), requestJson);
    Serial.printf("Sending measurements update: %s\n", requestJson);

    mqttClient.publish(getUpdateMeasurementsTopic().c_str(), requestJson);
}

template<typename TControls, typename TMeasurements>
void SmartHomeDevice<TControls, TMeasurements>::sendControlsSync() {
    char requestJson[MAX_MQTT_PACKET_SIZE];
    serializeJson(controlsProvider->getPayload(), requestJson);
    Serial.printf("Sending controls sync: %s\n", requestJson);

    mqttClient.publish(getSyncControlsTopic().c_str(), requestJson);
}

template<typename TControls, typename TMeasurements>
unsigned long SmartHomeDevice<TControls, TMeasurements>::msSinceLastPairRequest() {
    return millis() - lastPairRequest;
}

template<typename TControls, typename TMeasurements>
unsigned long SmartHomeDevice<TControls, TMeasurements>::msSinceLastUpdate() {
    return millis() - lastUpdate;
}

template<typename TControls, typename TMeasurements>
String SmartHomeDevice<TControls, TMeasurements>::getUpdateControlsTopic() {
    if (updateControlsTopic.length() == 0) {
        char topic[128];
        snprintf(topic, sizeof(topic), UPDATE_CONTROLS_TOPIC, deviceId.c_str());
        updateControlsTopic = topic;
    }
    return updateControlsTopic;
}

template<typename TControls, typename TMeasurements>
String SmartHomeDevice<TControls, TMeasurements>::getSyncControlsTopic() {
    if (syncControlsTopic.length() == 0) {
        char topic[128];
        snprintf(topic, sizeof(topic), SYNC_CONTROLS_TOPIC, deviceId.c_str());
        syncControlsTopic = topic;
    }
    return syncControlsTopic;
}

template<typename TControls, typename TMeasurements>
String SmartHomeDevice<TControls, TMeasurements>::getUpdateMeasurementsTopic() {
    if (updateMeasurementsTopic.length() == 0) {
        char topic[128];
        snprintf(topic, sizeof(topic), UPDATE_MEASUREMENTS_TOPIC, deviceId.c_str());
        updateMeasurementsTopic = topic;
    }
    return updateMeasurementsTopic;
}
