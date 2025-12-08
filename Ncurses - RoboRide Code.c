#include <wiringPi.h>
#include <softPwm.h>
#include <ncurses.h>
#include <stdio.h>

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
// ===============================================================

// Motor A
#define IN1 0
#define IN2 1
#define ENA 2

// Motor B
#define IN3 3
#define IN4 4
#define ENB 5

// Speed levels
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
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void backward(int speed) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void spin_left(int speed) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void spin_right(int speed) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

// ===============================================================

int main() {
    wiringPiSetup();

    // Setup motor pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    softPwmCreate(ENA, 0, 100);
    softPwmCreate(ENB, 0, 100);

    // Initialize Ncurses
    initscr();
    cbreak();
    noecho();
    printw("RoboRide NCURSES Controller\n");
    printw("Controls:\n");
    printw(" W - Forward\n S - Backward\n A - Left\n D - Right\n 1 - Slow Speed\n 2 - Fast Speed\n SPACE - Stop\n Q - Quit\n");
    refresh();

    int speed = SLOW_SPEED;
    char ch;

    while (1) {
        ch = getch(); // waits for key input (blocking)
        switch (ch) {
            case 'w': case 'W':
                forward(speed);
                printw("Moving Forward (%d%%)\n", speed);
                break;
            case 's': case 'S':
                backward(speed);
                printw("Moving Backward (%d%%)\n", speed);
                break;
            case 'a': case 'A':
                spin_left(speed);
                printw("Turning Left (%d%%)\n", speed);
                break;
            case 'd': case 'D':
                spin_right(speed);
                printw("Turning Right (%d%%)\n", speed);
                break;
            case '1':
                speed = SLOW_SPEED;
                printw("Speed set to SLOW (%d%%)\n", speed);
                break;
            case '2':
                speed = FAST_SPEED;
                printw("Speed set to FAST (%d%%)\n", speed);
                break;
            case ' ':
                motor_stop();
                printw("Stopped\n");
                break;
            case 'q': case 'Q':
                motor_stop();
                printw("Exiting...\n");
                endwin();
                return 0;
            default:
                printw("Invalid key\n");
                break;
        }
        refresh();
    }

    endwin();
    return 0;
}
