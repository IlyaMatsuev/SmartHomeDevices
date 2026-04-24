# Presence Sensor

Battery-friendly room presence sensor built on an ESP32 + HC-SR501 PIR. Reports motion to the smarthome hub over MQTT and spends most of its time in deep sleep.

## Build and flash

Run from this directory (`devices/presence-sensor/`):

```bash
# Build
pio run

# Upload over USB
pio run --target upload

# Serial monitor at 115200 baud
pio device monitor --baud 115200
```

## How it works

On every wake cycle the device:

1. Reads the PIR output pin (`digitalRead`). HIGH means the sensor is currently reporting motion.
2. Feeds the reading into `PresenceSensorMeasurements::setMotionDetection`, which applies a software cooldown (see below).
3. Connects to Wi-Fi and MQTT, pairs with the hub if not already paired, and publishes:
  - a controls sync (`home/devices/{id}/controls/sync`)
  - a measurements update (`home/devices/{id}/measurements/update`) containing `{"motion": true|false}`
4. Cleanly disconnects MQTT + Wi-Fi and goes back to deep sleep.

### Wake sources

Deep sleep ends on whichever comes first:

- **Timer** — always set, every `SLEEP_DURATION_MS` (default 60 s).
- **External (ext0) motion wake** — armed only when the current reported state is *not* motion. The ESP32 wakes immediately when the PIR pin goes HIGH, so transitions from "no motion" to "motion" are reported with near-zero latency instead of waiting up to a full sleep interval.

When motion is already reported, the timer is the only wake source - the device doesn't need to re-detect motion that it's already reporting.

### Software cooldown

The PIR sensor itself has a hardware hold time (set by the Tx pot). On top of that the firmware enforces a software cooldown: once motion is reported, the device keeps reporting `motion: true` for at least `MOTION_COOLDOWN_MS` (default 2 min) before it will consider returning to `false`. This smooths out gaps between PIR re-triggers and prevents flicker in the hub.

The cooldown counter is advanced each time the device sleeps (by the sleep duration) and persisted in RTC memory so it survives deep sleep.

### Controls

The device exposes one control:

- `on` (bool, default `true`) — when set to `false` by the hub, the device still wakes on its normal schedule but skips publishing measurement / sync updates. Useful for pausing the sensor without unplugging it.

## Hardware

### Components

- ESP32 dev board (project is pinned to `mhetesp32devkit`, any ESP32 works with a matching PlatformIO `board` setting)
- HC-SR501 PIR motion sensor (or pin-compatible clone)
- Jumper wires

### Wiring

| HC-SR501 pin | ESP32 pin       | Notes                                              |
| ------------ | --------------- | -------------------------------------------------- |
| VCC          | 5V / VIN        | HC-SR501 needs 5 V input; its onboard LDO feeds the PIR |
| GND          | GND             |                                                    |
| OUT          | GPIO 32         | `MOTION_SENSOR_PIN` in `src/main.cpp`              |

GPIO 32 is an RTC-capable pin, which is required because the firmware uses it as an `ext0` deep-sleep wake source. If you change the pin, pick another RTC GPIO (0, 2, 4, 12–15, 25–27, 32–39).

### HC-SR501 configuration

Two potentiometers and one jumper live on the solder side of the module (opposite the white dome). The silkscreen labels them but the text is tiny.

- **Sensitivity (Sx)** — detection range, roughly 3 m (CCW) to 7 m (CW).
- **Time Delay (Tx)** — how long OUT stays HIGH after motion, roughly 5 s (CCW) to ~5 min (CW). Set this based on how responsive you want the hardware signal to be; the firmware's own cooldown sits on top of it.
- **H / L jumper** — set to **H** (repeatable trigger). In H mode, sustained motion re-arms the Tx timer so OUT stays HIGH the whole time someone's present plus Tx afterward. L mode pulses once and locks out, which is not what you want for a presence sensor.

If you can't tell which pot is which, trigger the sensor and time how long OUT stays HIGH. Turn one pot ~½ turn clockwise and retest — whichever changes the HIGH duration is Tx.

## Configuration

Everything tunable lives at the top of `src/main.cpp`:

| Constant                              | Default    | Meaning                                                        |
| ------------------------------------- | ---------- | -------------------------------------------------------------- |
| `MOTION_SENSOR_PIN`                   | 32         | ESP32 GPIO wired to HC-SR501 OUT                               |
| `MOTION_SENSOR_CALIBRATION_TIMEOUT_MS`| 30 000     | PIR warm-up / settling time on cold boot only                  |
| `MOTION_COOLDOWN_MS`                  | 120 000    | Software cooldown — minimum time `motion:true` is reported     |
| `SLEEP_DURATION_MS`                   | 60 000     | Timer wake interval                                            |
| `DEVICE_NAME`                         | `presence-sensor` | MQTT client id / name in pair request                   |
| `DEVICE_TYPE`                         | `motion-sensor`   | Device type sent in pair request                        |

Network / MQTT credentials come from `devices/common/Secrets/src/secrets.cpp` (not committed). The contract is declared in `devices/common/Secrets/include/secrets.h`.
