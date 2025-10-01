📌 Overview

In Week 2, the focus shifted from initial setup (Week 1) to motor driver research and LED PWM control on the Raspberry Pi.
Key highlights:

Studied the L293D H-Bridge Motor Driver IC and its applications

L293D Presentation TD 2

.

Implemented PWM-based LED brightness control using C on the Raspberry Pi

blinko code- TD 2

.

Logged daily progress and challenges in the Week 2 journal

Week 2 Journal

.

📂 Contents
File	Description
blinko code- TD 2.c	C program to control LED brightness using PWM on Raspberry Pi with keyboard inputs

blinko code- TD 2


L293D Presentation TD 2.pptx	Presentation covering datasheet, pinout, specs, and truth table of the L293D motor driver

L293D Presentation TD 2


Week 2 Journal.pdf	Daily log of learning activities, debugging attempts, and project updates

Week 2 Journal

⚡ L293D Motor Driver (Summary)

Dual H-Bridge motor driver IC

Controls two DC motors (bidirectional)

Specs:

Motor supply (VS): 4.5–36 V

Logic supply (VSS): 4.5–5.5 V

600 mA continuous current / 1.2 A peak

Use Case: Interface between Raspberry Pi GPIO logic and higher-power DC motors

L293D Presentation TD 2

📸 L293D Pinout:


💡 LED PWM Control (C Program)
Features:

Uses WiringPi for GPIO control

Non-blocking keyboard input for real-time PWM duty cycle adjustment

Duty cycle options:

a → 25%

s → 50%

d → 75%

w → 90%

q → LED off

x → Exit program

blinko code- TD 2

Run Instructions:
# Compile
gcc blinko\ code-\ TD\ 2.c -lwiringPi -o pwm_led

# Run
./pwm_led

📓 Week 2 Journal Highlights

Sept 10: Setup Pi, tested Hello World, configured VNC viewer

Week 2 Journal

Sept 11: Researched PWM from BYJUS & Circuit Digest; completed presentation draft

Sept 12: Missed class but Nikunj presented; faced SSH issues with Pi

Sept 15: Learned L293D via YouTube, synced Pi, began LED coding task

Week 2 Journal

Sept 16: Completed assignments, Nikunj handled LED coding, delivered presentation

✅ Outcomes

Gained working knowledge of motor drivers (L293D) and their GPIO interface.

Implemented and tested PWM LED brightness control in C.

Overcame setup and SSH challenges with Raspberry Pi.

Continued collaborative work, presentations, and weekly documentation.

👨‍💻 Authors

Bilal Hussain

Nikunj Patel
