## RoboRide – Raspberry Pi Smart Robotic Car
<img width="622" height="629" alt="Screenshot 2025-11-11 215249" src="https://github.com/user-attachments/assets/ddfb69f8-77f3-489a-8726-6c8522013ab9" />

## Project Overview
RoboRide is a Raspberry Pi–based two-wheel robotic car designed to demonstrate real-time software control of hardware using GPIO and PWM signals. The robot supports directional movement, speed control, ultrasonic-based safety stopping in reverse, and camera-based photo/video capture — all controlled through a terminal-based user interface.

This project was developed as a Phase 1 & Phase 2 academic engineering project to apply embedded systems, motor control, and real-time programming concepts.

## Features

- Forward, backward, left, and right movement

- Two selectable speed modes (Slow & Fast)

- Automatic reverse stop using ultrasonic sensor

- Photo capture using Raspberry Pi Camera Module

- Video recording with start/stop control

- Terminal-based real-time control (Ncurses UI)

- Safe motor direction switching

- Dual power supply architecture
  
![WhatsApp Image 2025-12-04 at 01 40 49_76183bda](https://github.com/user-attachments/assets/219912e5-8fd9-4a3a-98dd-174ab593aebf)

## System Overview

Controller: Raspberry Pi (Zero W / 4 Model B)

Motor Driver: L293D Dual H-Bridge IC

Motors: 2 × DC Gear Motors

Sensor: HC-SR04 Ultrasonic Sensor (Rear Mounted)

Camera: Raspberry Pi Camera Module 2

User Interface: Linux Terminal (Ncurses)

Programming Language: C

Libraries Used: WiringPi, softPwm, ncurses

## 🎮 Control Keys (WASD Standard)
Key	Function
W	Move Forward
S	Move Backward
A	Turn Left
D	Turn Right
SPACE	Stop
1	Slow Speed
2	Fast Speed
C	Capture Photo
V	Start Video Recording
B	Stop Video Recording
Q	Quit Program

⚠️ Automatic Safety:
If the robot is reversing and an obstacle is detected within 50 cm, the robot will stop automatically.

## 🔌 Hardware Connections
Motor Driver (L293D)

IN1 → GPIO 17

IN2 → GPIO 18

ENA → GPIO 27

IN3 → GPIO 22

IN4 → GPIO 23

ENB → GPIO 24

Ultrasonic Sensor

TRIG → GPIO 5

ECHO → GPIO 12 (with voltage divider)

Camera

Connected to the CSI Camera Port

## 🛠️ Software Installation
1️⃣ Update System
sudo apt update && sudo apt upgrade

2️⃣ Install Required Libraries
sudo apt install wiringpi libncurses5-dev

3️⃣ Enable Camera
sudo raspi-config


Enable Camera Interface, then reboot.

⚙️ Compile and Run
Compile:
gcc robo.c -o robo -lwiringPi -lncurses

Run:
sudo ./robo

## 📁 Media Storage

📸 Photos saved to:

~/Pictures/


🎥 Videos saved to:

~/Videos/


All media files are automatically timestamped.

## 📌 Project Phases
✅ Phase 1

Chassis setup

Manual motor control

L293D motor driver integration

Terminal-based control logic

✅ Phase 2

Ultrasonic safety stopping

Camera integration

Video recording support

Enhanced UI

Improved power management

## ⚠️ Limitations

Light-load motors only

Manual control only (no wireless yet)

Battery runtime is limited

Not suitable for rough terrain

## 🔮 Future Enhancements

✅ Wireless control via Wi-Fi or Bluetooth

✅ Autonomous obstacle avoidance

✅ Mobile app integration

✅ AI-based object detection

✅ Li-ion battery system with regulator

## 👨‍💻 Developed By

Bilal Hussain Mohammed

Engineering Student – Electronics System Engineering

GitHub: Bilal2866

## ⭐ How to Support

If you found this project useful:

✅ Star the repository

✅ Fork it

✅ Build your own version

✅ Improve it
