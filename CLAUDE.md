# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

ESP32 smart home devices built with PlatformIO and the Arduino framework. Each device under `devices/` is an independent PlatformIO project that communicates with a central hub via MQTT (using JSON payloads over PubSubClient).

## Build Commands

Each device is its own PlatformIO project. Run commands from the device directory (e.g., `devices/ps-fans/`):

```bash
# Build
pio run

# Upload to connected ESP32
pio run --target upload

# Serial monitor (115200 baud)
pio device monitor --baud 115200
```

## Architecture

**Shared libraries** (`devices/common/`, included via `lib_extra_dirs` in each device's `platformio.ini`):

- **SmartHomeDevice** — Base framework handling WiFi connection, MQTT pairing protocol, and periodic measurement/control sync. Each device creates a `SmartHomeDevice` instance with its own `MeasurementsProvider` and `ControlsProvider` implementations.
- **Secrets** — Declares extern constants for WiFi and MQTT credentials (`secrets.h`). You must create `devices/common/Secrets/src/secrets.cpp` with actual values (not committed).

**MQTT protocol flow**: Device sends pair request to `home/devices/pair` -> receives device ID and config from `home/devices/pair/reply` -> subscribes to `home/devices/{id}/controls/update` for inbound control changes -> publishes measurements to `home/devices/{id}/measurements/update` on interval -> syncs controls to `home/devices/{id}/controls/sync` on change.

**Adding a new device**: Create a new directory under `devices/` with its own `platformio.ini`, `src/main.cpp`, and header/source files. Implement `MeasurementsProvider` (override `build()`) and `ControlsProvider` (override `build()` and `onUpdate()`) for device-specific data. Reference common libs via `lib_extra_dirs = ../common`.

## Current Devices

- **ps-fans** — Temperature-reactive fan controller (Dallas DS18B20 sensor + PWM fan). Has multi-level speed thresholds with hysteresis.
- **presence-sensor** — Motion detection via PIR sensor.
