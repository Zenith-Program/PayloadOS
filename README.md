# NASA Student Launch Payload – Teensy 4.1 Flight Software

This repository contains the embedded software for the payload system flown in the **2025 NASA Student Launch Competition**. Thie project was developed using [PlatformIO](https://platformio.org/) and written for the **Teensy 4.1** microcontroller. The project encompasses flight tracking algorithims, sensor integration, data logging, VHF APRS messaging, hardware in the loop testing, and a command line interface for high-power rocketry.

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
- **MS5706 Barometric Altimeter**: 
- **BNO055 IMU**: 
- **BNO080 IMU**: 
- **LightAPRS Transmitter Module**:

## 📈 Telemetry Format
Telemetry is stored in .txt files with entries seperated by spaces and newlines. [Here](README.md) is an example file. A .csv MATLAB conversion script found here can be used to reformat the generated .txt files. An event log is also maintained durring flight as a .txt file. 
# Future Teams' Reference
The PayloadOS program can run with just a Teensy 4.1 board with limited functionality. A full HIL flight simulation can be conducted without any of the peripheral modules.
## Environment Setup


