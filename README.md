# Project Overview: USB and BLE Connectivity with NRF52840

This project is designed to get more familiar with the USB capabilities of the NRF52840 microcontroller. It leverages the HID class to function as both a keyboard and mouse, additionally, it incorporates Bluetooth Low Energy (BLE) connectivity using the Nordic UART Service (NUS).

By using the Serial Bluetooth Terminal app from Google Play(I used that one), you can connect your mobile phone to the NRF52840. Once connected, any text sent from your mobile phone is instantly displayed on your computer. Moreover, you can control the mouse using specific commands sent from your mobile device.
To enable the NRF52840 to act as a keyboard and mouse, you need to connect it to your computer via USB, allowing it to emulate these devices and display its ports.

Mouse Control Commands via Mobile Phone:
Move Up: up:<0-1000>
Move Down: down:<0-1000>
Move Right: right:<0-1000>
Move Left: left:<0-1000>


## Instructions for Setting Up the Project
Step 1: Build the Docker Image

To set up the NRF Connect SDK environment, build the Docker image using the following command:
```
docker build -t nrfconnect-sdk --build-arg sdk_nrf_version=v2.6-branch .
```
Step 2: Run the Docker Container

Run the Docker container with the necessary privileges and device access. Replace <nrf52840 device when connected> with the actual device identifier (e.g., /dev/ttyACM0):
```
docker run -it --privileged --device=<nrf52840 device when connected> -v ${PWD}:/workdir/project nrfconnect-sdk /bin/bash
```

Step 3: Use Python Scripts for Development

From there, you can use Python scripts in the /scripts folder to build, flash, and monitor your development.

To build:

```
./scripts/build.py
```

To flash:

```
./scripts/flash.py
```

To monitor:

```
./scripts/monitor.py --port <device_port>
```

To clean the build folder:

```
./scripts/clean.py
```

These steps will set up the necessary environment to develop and test your NRF52840 project efficiently.

Project was done on Ubuntu using Nrf52840 development kit.