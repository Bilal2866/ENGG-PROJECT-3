#include <wiringPi.h>
#include <softPwm.h>
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
// Power
//   Vcc1 (pin 16 on L293D) → Pi 5V (physical pin 2 or 4)
//   Vcc2 (pin 8 on L293D)  → Motor battery + (~6V)
//   GND  (pins 4,5,12,13) → Common ground (Pi + battery -)
//
// ===============================================================

// Motor A
#define IN1 0   // GPIO17, Pin 11
#define IN2 1   // GPIO18, Pin 12
#define ENA 2   // GPIO27, Pin 13

// Motor B
#define IN3 3   // GPIO22, Pin 15
#define IN4 4   // GPIO23, Pin 16
#define ENB 5   // GPIO24, Pin 18

// Speed presets (duty cycles)
#define SLOW_SPEED 70   // ~70% (tune for non-jittery slow)
#define FAST_SPEED 90   // ~90% (tune for controllable fast)

// ================= Utility Functions =================

// Stop both motors (set pins LOW, duty=0)
void motor_stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, 0);
    softPwmWrite(ENB, 0);
    delay(100);  // safety delay before changing direction
}

// Forward movement
void forward(int speed) {
    motor_stop();
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

// Backward movement
void backward(int speed) {
    motor_stop();
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

// Spin left in place
void spin_left(int speed) {
    motor_stop();
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

// Spin right in place
void spin_right(int speed) {
    motor_stop();
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

    softPwmCreate(ENA, 0, 100);  // duty range 0-100
    softPwmCreate(ENB, 0, 100);

    char cmd[10];  // input buffer for commands
    int speed = SLOW_SPEED;  // start at slow speed
    int running = 1;

    printf("Robot Motor Controller\n");
    printf("Commands:\n");
    printf(" FWD  = forward\n");
    printf(" BCK  = backward\n");
    printf(" LFT  = spin left\n");
    printf(" RGT  = spin right\n");
    printf(" STOP = stop\n");
    printf(" 1    = set slow speed (%d%%)\n", SLOW_SPEED);
    printf(" 2    = set fast speed (%d%%)\n", FAST_SPEED);
    printf(" EXIT = quit program\n");

    while (running) {
        printf("\nEnter command: ");
        scanf("%s", cmd);

        if (strcmp(cmd, "FWD") == 0) {
            forward(speed);
            printf("Moving forward at %d%% duty\n", speed);
        }
        else if (strcmp(cmd, "BCK") == 0) {
            backward(speed);
            printf("Moving backward at %d%% duty\n", speed);
        }
        else if (strcmp(cmd, "LFT") == 0) {
            spin_left(speed);
            printf("Spinning left at %d%% duty\n", speed);
        }
        else if (strcmp(cmd, "RGT") == 0) {
            spin_right(speed);
            printf("Spinning right at %d%% duty\n", speed);
        }
        else if (strcmp(cmd, "STOP") == 0) {
            motor_stop();
            printf("Stopped\n");
        }
        else if (strcmp(cmd, "1") == 0) {
            speed = SLOW_SPEED;
            printf("Speed set to SLOW (%d%%)\n", speed);
        }
        else if (strcmp(cmd, "2") == 0) {
            speed = FAST_SPEED;
            printf("Speed set to FAST (%d%%)\n", speed);
        }
        else if (strcmp(cmd, "EXIT") == 0) {
            motor_stop();
            running = 0;
            printf("Exiting program...\n");
        }
        else {
            printf("Invalid command!\n");
        }
    }

    return 0;
}