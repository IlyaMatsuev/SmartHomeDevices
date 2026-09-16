#include <Arduino.h>
#include <MyLD2410.h>

const uint8_t RADAR_RX_PIN = 27;
const uint8_t RADAR_TX_PIN = 26;
//const uint8_t RADAR_OUT_PIN = 35;


// 4.5 – 5.25m. How far the sensor looks for moving targets
const uint8_t MAX_MOVING_GATE = 5;
// 2.25 – 3m. How far the sensor looks for stationary targets
const uint8_t MAX_STATIONARY_GATE = 4;
// Duration in seconds until the HIGH turns back to LOW if noone detected
const uint8_t NO_ONE_DURATION = 5;

const uint8_t TOTAL_SENSOR_GATES = 9;
// 0.75m gates; threshold a gate signal must exceed to trigger detection.
// 0 = always triggers, 100 = least sensitive. Use setMaxGate() to disable far gates
uint8_t movingThresholds[] = {100, 100, 50, 40, 10, 10, 100, 100, 100};
uint8_t stationaryThresholds[] = {100, 100, 90, 90, 100, 100, 100, 100, 100};

MyLD2410 sensor(Serial1);

class MyMyLD2410 : public MyLD2410 {
    
}


void printParameters(bool first);
void printData();
void printValue(const byte &val);


void setup() {
    Serial.begin(115200);
    Serial1.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
    //pinMode(RADAR_OUT_PIN, INPUT);

    delay(2000);

    if (!sensor.begin()) {
        while (true) {
            Serial.println("Failed to communicate with the sensor.");
            delay(2000);
        }
    }

    sensor.setMaxGate(MAX_MOVING_GATE, MAX_STATIONARY_GATE, NO_ONE_DURATION);
    for (uint8_t gate = 0; gate < TOTAL_SENSOR_GATES; gate++) {
        sensor.setGateParameters(gate, movingThresholds[gate], stationaryThresholds[gate]);
    }
    sensor.enhancedMode();

    Serial.println("Sensor configured");
    printParameters(true);

    delay(20);
}

void loop() {
    static unsigned long lastPrint = 0;

    if ((sensor.check() == MyLD2410::Response::DATA) && (millis() - lastPrint >= 2000)) {
        lastPrint = millis();
        printData();
    }

    // if (radar.isConnected()) {
    //     Serial.printf("UART presence: %s | Moving target distance: %d | Stationary target distance: %d\n", radar.presenceDetected() ? "true" : "false", radar.movingTargetDistance(), radar.stationaryTargetDistance());
    // } else {
    //     Serial.print("Radar not connected");
    // }

    //delay(2000);
}


void printParameters(bool first = false) {
    sensor.configMode();
    sensor.requestParameters();
    if (first) {
        Serial.print("Firmware: ");
        Serial.println(sensor.getFirmware());
        Serial.print("Protocol version: ");
        Serial.println(sensor.getVersion());
        Serial.print("Bluetooth MAC address: ");
        Serial.println(sensor.getMACstr());
    }
    const MyLD2410::ValuesArray &mThr = sensor.getMovingThresholds();
    const MyLD2410::ValuesArray &sThr = sensor.getStationaryThresholds();

    Serial.print("Resolution (gate-width): ");
    Serial.print(sensor.getResolution());
    Serial.print("cm\nMax range: ");
    Serial.print(sensor.getRange_cm());
    Serial.print("cm\nMoving thresholds    [0,");
    Serial.print(mThr.N);
    Serial.print("]:");
    mThr.forEach(printValue);
    Serial.print("\nStationary thresholds[0,");
    Serial.print(sThr.N);
    Serial.print("]:");
    sThr.forEach(printValue);
    Serial.print("\nNo-one window: ");
    Serial.print(sensor.getNoOneWindow());
    Serial.println('s');

    sensor.configMode(false);
}

void printData() {
    Serial.print("Data frame #: ");
    Serial.println(sensor.getFrameCount());
    Serial.print("Time stamp [ms]: ");
    Serial.println(sensor.getTimestamp());
    Serial.print(sensor.statusString());
    if (sensor.presenceDetected()) {
        Serial.print(", distance: ");
        Serial.print(sensor.detectedDistance());
        Serial.print("cm");
    }
    
    Serial.println();
    if (sensor.inEnhancedMode()) {
        Serial.print("\nMoving signals->[");
        sensor.getMovingSignals().forEach(printValue);
        Serial.print(" ] thresholds:[");
        sensor.getMovingThresholds().forEach(printValue);
        Serial.print(" ]");
    }
    Serial.println();

    Serial.println();
    if (sensor.inEnhancedMode()) {
        Serial.print("\nStationary signals->[");
        sensor.getStationarySignals().forEach(printValue);
        Serial.print(" ] thresholds:[");
        sensor.getStationaryThresholds().forEach(printValue);
        Serial.print(" ]");
    }
    Serial.println();
    // if (sensor.movingTargetDetected()) {
    //     Serial.print(" MOVING    = ");
    //     Serial.print(sensor.movingTargetSignal());
    //     Serial.print("@");
    //     Serial.print(sensor.movingTargetDistance());
    //     Serial.print("cm ");
    //     if (sensor.inEnhancedMode()) {
    //         Serial.print("\n signals->[");
    //         sensor.getMovingSignals().forEach(printValue);
    //         Serial.print(" ] thresholds:[");
    //         sensor.getMovingThresholds().forEach(printValue);
    //         Serial.print(" ]");
    //     }
    //     Serial.println();
    // }
    // if (sensor.stationaryTargetDetected()) {
    //     Serial.print(" STATIONARY= ");
    //     Serial.print(sensor.stationaryTargetSignal());
    //     Serial.print("@");
    //     Serial.print(sensor.stationaryTargetDistance());
    //     Serial.print("cm ");
    //     if (sensor.inEnhancedMode()) {
    //     Serial.print("\n signals->[");
    //     sensor.getStationarySignals().forEach(printValue);
    //     Serial.print(" ] thresholds:[");
    //     sensor.getStationaryThresholds().forEach(printValue);
    //     Serial.print(" ]");
    //     }
    //     Serial.println();
    // }

    if (sensor.inEnhancedMode() && (sensor.getFirmwareMajor() > 1)) {
        Serial.print("Light level: ");
        Serial.println(sensor.getLightLevel());
        Serial.print("Output level: ");
        Serial.println((sensor.getOutLevel()) ? "HIGH" : "LOW");
    }

    Serial.println();
}

void printValue(const byte &val) {
  Serial.print(' ');
  Serial.print(val);
}
