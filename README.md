# Automotive-Control-System
This project designed and implemented an IoT-based embedded system using Raspberry Pi, ADXL345 accelerometer, and GPS module for real-time accident detection. Data is transmitted via MQTT protocol to a Flask backend that provides REST APIs consumed by a React + TypeScript dashboard for live visualization and analytics. Integrated UART communication, paho-mqtt, and pynmea2 for reliable sensor data parsing and messaging.

# System Architecture
Three-layer IoT architecture that integrates embedded hardware, cloud communication, and a web-based dashboard

-Perception Layer: Sensors (ADXL345 + GPS) detect events and collect data.
-Network Layer: Raspberry Pi transmits accident and GPS data via MQTT to the broker.
-Application Layer: Flask backend processes and serves the data to the React dashboard for real-time monitoring.

