# DroneTools

**DroneTools** is a modular, high-performance toolkit for drone flight
control, sensor fusion, and real-time telemetry visualization.

Built for microcontrollers like **ESP32**, **ESP8266**, and **RP2040**,
it provides everything from low-level ESC drivers to a fully interactive
3D web-based monitoring dashboard.

------------------------------------------------------------------------

## Features

### Firmware & Flight Control

-   **Multicore Support**\
    Optimized for ESP32 and RP2040 architectures.

-   **Advanced AHRS**\
    Kalman and Madgwick filters for precise orientation tracking.

-   **Protocol Support**\
    Compatible with CRSF, IBUS, SBUS, and PPM.

-   **Blackbox Logging**\
    Serial and Flash-based logging for post-flight analysis.

-   **Modular Sensor Support**

    -   Gyroscopes: MPU6050, MPU9250, BMI160\
    -   Barometers: BMP280, SPL06\
    -   GPS modules

-   **ESC Drivers**\
    Hardware-specific drivers for ESP and RP2040 platforms.

------------------------------------------------------------------------

### Real-time Telemetry Visualizer

-   **3D Visualization**\
    Built with Three.js to display real-time pitch, roll, and yaw.

-   **Live Graphing**\
    Streams IMU and aerodynamic data via WebSockets.

-   **Modular Dashboard**\
    Interactive UI for monitoring system health and sensors.

-   **Python Bridge**\
    Serial-to-WebSocket bridge for seamless telemetry streaming.

------------------------------------------------------------------------

## Project Structure

    DroneTools/
    │
    ├── drone-telemetry/     # Web-based 3D visualizer (HTML/JS/Three.js)
    ├── lib/
    │   ├── AHRS/            # Kalman & Madgwick filters
    │   ├── Espfc/           # Core flight controller logic (PID, mixers, RC, sensors)
    │   └── EscDriver/       # Motor control abstraction layers
    │
    ├── bridge.py            # Serial → WebSocket bridge
    ├── readings.ino         # Main firmware (sensor acquisition)
    └── README.md

------------------------------------------------------------------------

## Getting Started

### 1. Flash the Firmware

-   Open the project in **Arduino IDE** or **PlatformIO**
-   Install board support for ESP32 / RP2040
-   Configure your pins in `readings.ino`
-   Upload to your flight controller

------------------------------------------------------------------------

### 2. Run the Telemetry Bridge

Install dependencies:

    pip install pyserial websockets

Run the bridge:

    python bridge.py --port COM_PORT --baud 115200

Replace `COM_PORT` with your actual serial port.

------------------------------------------------------------------------

### 3. Open the Visualizer

-   Navigate to: `drone-telemetry/index.html`
-   Open it in any modern browser
-   It will automatically connect to the local WebSocket server

------------------------------------------------------------------------

## Tech Stack

-   **Firmware:** C++ / Arduino (ESP-IDF / FreeRTOS)\
-   **Bridge:** Python (Asyncio, WebSockets)\
-   **Dashboard:** JavaScript, Three.js, HTML5, CSS3\
-   **Protocols:** MSP (MultiWii Serial Protocol), CRSF

------------------------------------------------------------------------

## Advanced Features

### Sensor Fusion

Located in `lib/AHRS/`: - Custom Madgwick filter\
- 1D / 2D Kalman filters

These convert noisy IMU data into stable orientation estimates.

------------------------------------------------------------------------

### Aerodynamics Simulation

Implemented in: `drone-telemetry/systems/aerodynamics.js`

-   Simulates airframe stress\
-   Visualizes lift distribution\
-   Driven by live telemetry data

