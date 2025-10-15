📌 Overview (Week 4)

In Week 4, we moved from planning into full hardware implementation and testing. We mounted the Raspberry Pi and breadboard on the robot chassis, wired the L293D motor driver, soldered unstable connections, and wrote C code to control motor direction and speed using PWM. We also fixed wiring issues, tested PWM output, and successfully made the robot move.

✅ Hardware fully assembled
✅ Motor control code written and tested
✅ PWM used for speed control
✅ Soldered connections for stability
✅ Troubleshot wiring and IC issues
✅ Robot successfully moved

📂 Files Included

Assets TD 4.zip – Images, reference files, or supporting materials used for Week 4

TD 4 car code.c – Final code for DC motor control using PWM + direction (WASD style)

Week 4 Journal.pdf – Daily log of tasks, testing, wiring, coding, and troubleshooting

README.md – Summary of Week 4 work

🔧 Hardware Work

Mounted Raspberry Pi and breadboard on chassis

Tested both DC motors

Soldered motor wires and battery switch to fix loose connections

Used power bank and AA battery pack as power sources

Secured wiring and components with double tape

Debugged wiring mistakes and IC issues

💡 Coding Work

Versions of motor control code:

✅ Basic test (forward/reverse for 2 seconds)
✅ Direction control using keys (W, A, S, D)
✅ Final code: PWM speed control + direction + safety stop

Features of final code:

Forward / Backward / Left / Right

Speed levels using PWM

Stops motor before reversing (safety)

🧪 Testing & Debugging

Checked motor outputs individually

Replaced L293D IC when motors stopped working

Tested PWM using LED to verify duty cycle

Found and fixed incorrect wiring

After rewiring, robot started moving correctly

✅ Outcomes

✅ Full motor control using Raspberry Pi + L293D
✅ PWM speed control worked correctly
✅ Wiring and power issues solved
✅ Hardware setup was stable after soldering
✅ Robot successfully moved on the floor
✅ Strong progress in both coding and hardware

👨‍💻 Team Members

Bilal Hussain (ID: 9042866)
Nikunj Patel (ID: 9047741)
