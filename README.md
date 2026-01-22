# SmartHome Devices

This repo contains the source code of the custom microcontroller devices for my SmartHome ecosystem.

Each device is located under a separate folder in `/devices`, and each device has their own `.pio` file so that it can opened, compiled, and uploaded to a device via Platform IO.

## 🛠️ Requisites

- Create a `secrets.cpp` file at `/devices/common/Secrets/src` with all the secrets described in the related [`secrets.h`](/devices/common/Secrets/include/secrets.h) file
