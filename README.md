# 🚗 RTOS Robotic Car Project 🚗

Welcome to the **RTOS Robotic Car Project**, where real-time operating systems meet the world of robotics! This project integrates an RTOS to control a fully functional robotic car with motor control, LED feedback, audio alerts, and remote control via an Android app over Wi-Fi. The system is designed for real-time precision and can be customized for various robotic and embedded system applications.

---

## 🚀 Project Features

- **🕹️ Motor Control**: Precision control over the car’s movement using PWM signals (see `PWN.h`).
- **🌈 LED Feedback**: Real-time status indication through the car’s LED system, configured in `Led.h`.
- **🔊 Audio Alerts**: The system includes real-time audio output to notify the user of specific states or errors.
- **📶 Wi-Fi Connectivity**: Remote control functionality via Wi-Fi, using UART for communication (`UART.h`).
- **🔁 Task Scheduling**: Core task management through RTOS scheduling to control motor, LED, and other real-time tasks.

---

## 🛠️ Project Components

### Hardware:
- **Robotic Car Kit**: Including motors and wheels, and 3D-printed acrylic chassis.
- **Wi-Fi Module**: ESP32 module used for communication.
- **LEDs**: Array LED module used for real-time feedback.
- **Speakers**: Piezo buzzer module used to produce audio alerts.
- **Android Device**: Android app built with MIT App Inventor to control the robot.

### Software:
- **RTOS**: Handles task scheduling and inter-task communication.
- **C Code**: Core functionalities divided into headers and source files:
  - `main.c`: Initializes and orchestrates the RTOS tasks.
  - `Directions.h`: Handles direction-based movement of the car.
  - `Led.h`: Configures and manages the LED system.
  - `PIT.h`: Programmable Interrupt Timer configuration.
  - `PWM.h`: Pulse Width Modulation for motor control.
  - `UART.h`: Manages UART communication for Wi-Fi control.
  - `EventRecorderStub.scvd`: Stub for event recording.

---
