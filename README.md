# NASA Student Launch Payload – Teensy 4.1 Flight Software

This repository contains the embedded software for the payload system flown in the **2025 NASA Student Launch Competition**. The project was developed using [PlatformIO](https://platformio.org/) and written for the **Teensy 4.1** microcontroller. It encompasses flight tracking algorithims, sensor integration, data logging, VHF APRS messaging, hardware in the loop testing, and a command line interface for high-power rocketry.

## 🚀 Overview

- **Target Platform**: Teensy 4.1 (ARM Cortex-M7)
- **Environment**: PlatformIO (with Arduino framework)
- **Purpose**: Autonomous flight payload in a high-powered rocketry environment.
- **Competition**: [NASA Student Launch 2025](https://www.nasa.gov/stem/studentlaunch/home/index.html)

> This project was developed by Nathan J. Hardie for participation in the NASA Student Launch Initiative.

---

## 📁 Repository Structure

```bash
.
├── include/            # Header files
├── lib/                # External and shared libraries
├── src/                # Core source code
│   ├── main.cpp        # Main entry point
│   └── ...             # Other modules (sensors, logging, control)
├── test/               # Unit tests (if any)
├── platformio.ini      # PlatformIO configuration
└── README.md           # You're here
```

## 🛠️ Required Hardware
The payload system relies on the follwoing external hardware besides Teensy 4.1. 
- **MS5706 Barometric Altimeter**: Connected to Teensy 4.1's I2C bus #1. It an be found [here](https://www.parallax.com/product/altimeter-module-ms5607/).
- **BNO055 IMU**: Connected to Teensy 4.1's I2C bus #2. It can be found [here](https://www.adafruit.com/product/2472).
- **BNO080 IMU**: Four of these (one per STEMnaut) are used. Two on I2C bus #2 and one on I2C bus #1. It can be found [here](https://www.ceva-ip.com/product/fsm-9-axis-module/).
- **LightAPRS Transmitter Module**: Used as a secondary altimeter, battery monitor, GPS (not fully implemented), and VHF transmitter. Connected to Teensy 4.1's I2C bus #3. It can be found [here](https://www.qrp-labs.com/lightaprs.html).

# Future Teams' Reference
The PayloadOS program can run with just a Teensy 4.1 board with limited functionality. A full HIL flight simulation can be conducted without any of the peripheral modules.
## Environment Setup

## Telemetry Format
Telemetry is stored in .txt files with entries seperated by spaces and newlines. [Here](README.md) is an example file. A .csv MATLAB conversion script found here can be used to reformat the generated .txt files. An event log is also maintained durring flight as a .txt file. 

## Reccomendations


