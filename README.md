# NASA Student Launch Payload – Teensy 4.1 Flight Software

This repository contains the embedded software for the payload system flown in the **2025 NASA Student Launch Competition**. Developed using [PlatformIO](https://platformio.org/) and written for the **Teensy 4.1** microcontroller, this project encompasses flight tracking algorithims, VHF APRS messaging, sensor integration, data logging, hardware in the loop testing, and a command line interface for high-power rocketry.

## 🚀 Overview

- **Target Platform**: Teensy 4.1 (ARM Cortex-M7)
- **Environment**: PlatformIO (with Arduino framework)
- **Purpose**: Autonomous control flight payload in a high-powered rocketry environment.
- **Competition**: [NASA Student Launch 2025](https://www.nasa.gov/stem/studentlaunch/home/index.html)

> This project was developed by the Nathan J. Hardie for participation in the NASA Student Launch Initiative.

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
