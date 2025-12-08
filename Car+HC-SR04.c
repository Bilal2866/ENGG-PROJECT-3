#include <wiringPi.h>
#include <softPwm.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

// ========================= Pin Mapping =========================
//
// Motor A (Left)
//   IN1 → WiringPi 0 → BCM 17 → Physical Pin 11
//   IN2 → WiringPi 1 → BCM 18 → Physical Pin 12
//   ENA → WiringPi 2 → BCM 27 → Physical Pin 13
//
// Motor B (Right)
//   IN3 → WiringPi 3 → BCM 22 → Physical Pin 15
//   IN4 → WiringPi 4 → BCM 23 → Physical Pin 16
//   ENB → WiringPi 5 → BCM 24 → Physical Pin 18
//
// Ultrasonic HC-SR04 (your wiring)
//   TRIG → BCM 5  → WiringPi 21 → Physical Pin 29
//   ECHO → BCM 12 → WiringPi 26 → Physical Pin 32 (via 1k/2k divider)
//
// ===============================================================

// Motor pins (WiringPi)
#define IN1 0
#define IN2 1
#define ENA 2
#define IN3 3
#define IN4 4
#define ENB 5

// Ultrasonic pins (WiringPi)
#define TRIG 21     // BCM5
#define ECHO 26     // BCM12

// Speed presets
#define SLOW_SPEED 70
#define FAST_SPEED 90

// ================= Motor Control Functions =================

void motor_stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, 0);
    softPwmWrite(ENB, 0);
    delay(100);  // safety delay
}

void forward(int speed) {
    motor_stop();
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void backward(int speed) {
    motor_stop();
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void spin_left(int speed) {
    motor_stop();
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void spin_right(int speed) {
    motor_stop();
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

// ================= Ultrasonic Distance Function =================

float getDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    // 10 µs trigger pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    // Wait for echo start
    unsigned long start = micros();
    while (digitalRead(ECHO) == LOW) {
        if (micros() - start > 300000) return -1.0f; // timeout
    }
    unsigned long echo_start = micros();

    // Wait for echo end
    while (digitalRead(ECHO) == HIGH) {
        if (micros() - echo_start > 300000) return -1.0f; // timeout
    }
    unsigned long echo_end = micros();

    // Calculate distance
    unsigned long travel_time = echo_end - echo_start;
    float distance = travel_time / 58.0f;

    if (distance < 2 || distance > 400)
        return -1.0f; // out of range
    return distance;
}

// ===============================================================

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    // Setup motor pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    softPwmCreate(ENA, 0, 100);
    softPwmCreate(ENB, 0, 100);
    motor_stop();

    // Setup ultrasonic pins
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    digitalWrite(TRIG, LOW);
    delay(50);

    // Initialize Ncurses
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);      // getch() is NON-BLOCKING
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "RoboRide NCURSES Controller + Ultrasonic Sensor");
    mvprintw(1, 0, "W - Forward | S - Backward | A - Left | D - Right");
    mvprintw(2, 0, "1 - Slow | 2 - Fast | SPACE - Stop | Q - Quit");
    refresh();

    int speed = SLOW_SPEED;
    int ch;
    unsigned long last_dist_ms = 0;

    while (1) {
        // ---- Handle keyboard input (non-blocking) ----
        ch = getch();
        if (ch != ERR) {
            switch (ch) {
                case 'w': case 'W':
                    forward(speed);
                    mvprintw(4, 0, "Motion: FORWARD   Speed: %d%%   ", speed);
                    break;
                case 's': case 'S':
                    backward(speed);
                    mvprintw(4, 0, "Motion: BACKWARD  Speed: %d%%   ", speed);
                    break;
                case 'a': case 'A':
                    spin_left(speed);
                    mvprintw(4, 0, "Motion: LEFT      Speed: %d%%   ", speed);
                    break;
                case 'd': case 'D':
                    spin_right(speed);
                    mvprintw(4, 0, "Motion: RIGHT     Speed: %d%%   ", speed);
                    break;
                case '1':
                    speed = SLOW_SPEED;
                    mvprintw(5, 0, "Speed mode: SLOW            ");
                    break;
                case '2':
                    speed = FAST_SPEED;
                    mvprintw(5, 0, "Speed mode: FAST            ");
                    break;
                case ' ':
                    motor_stop();
                    mvprintw(4, 0, "Motion: STOPPED             ");
                    break;
                case 'q': case 'Q':
                    motor_stop();
                    endwin();
                    return 0;
                default:
                    mvprintw(5, 0, "Invalid key                 ");
                    break;
            }
        }

        // ---- Read ultrasonic every 2 seconds ----
        unsigned long now = millis();
        if (now - last_dist_ms > 2000) {
            last_dist_ms = now;
            float d = getDistance();
            if (d < 0) {
                mvprintw(7, 0, "Distance: --- (no echo)        ");
            } else {
                mvprintw(7, 0, "Distance: %6.2f cm             ", d);
            }
        }

        refresh();
        delay(10); // small delay so we don't max CPU
    }

    endwin();
    return 0;
}
