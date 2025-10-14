#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <string.h>

//
// =============================================================
//               ROBORIDE MOTOR CONTROL PROGRAM
// =============================================================
// This program controls a 2-wheel robot using a Raspberry Pi
// and an L293D motor driver. The robot can move forward,
// backward, and spin left or right using terminal commands.
//
// =============================================================
// Hardware Summary:
//   Controller  : Raspberry Pi
//   Driver IC   : L293D
//   Motors      : 2x DC motors (Left & Right)
//   Power       : 5000 mAh power bank (Pi) + 4 AA batteries (motors)
// =============================================================


// ========================= Pin Mapping =========================
//
// Motor A (Left Side):
//   IN1 → WiringPi 0 → BCM 17 → Physical Pin 11
//   IN2 → WiringPi 1 → BCM 18 → Physical Pin 12
//   ENA → WiringPi 2 → BCM 27 → Physical Pin 13
//
// Motor B (Right Side):
//   IN3 → WiringPi 3 → BCM 22 → Physical Pin 15
//   IN4 → WiringPi 4 → BCM 23 → Physical Pin 16
//   ENB → WiringPi 5 → BCM 24 → Physical Pin 18
//
// Power Connections:
//   Vcc1 (L293D pin 16) → Pi 5V (logic supply)
//   Vcc2 (L293D pin 8)  → Motor battery + (~6V)
//   GND  (L293D pins 4,5,12,13) → Common ground (Pi GND + Battery −)
//
// ===============================================================


// Define pin numbers for Motor A (Left)
#define IN1 0   // GPIO17, Physical Pin 11
#define IN2 1   // GPIO18, Physical Pin 12
#define ENA 2   // GPIO27, Physical Pin 13

// Define pin numbers for Motor B (Right)
#define IN3 3   // GPIO22, Physical Pin 15
#define IN4 4   // GPIO23, Physical Pin 16
#define ENB 5   // GPIO24, Physical Pin 18

// Speed presets (duty cycles for PWM)
#define SLOW_SPEED 70   // ~70% duty (smooth slow speed)
#define FAST_SPEED 90   // ~90% duty (fast but controllable)


// ===============================================================
//                Motor Control Utility Functions
// ===============================================================

// Stop both motors completely
void motor_stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, 0);
    softPwmWrite(ENB, 0);
    delay(100);  // short delay before direction change (safety)
}


// Move the robot forward
void forward(int speed) {
    motor_stop(); // ensure motors are off before changing direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}


// Move the robot backward
void backward(int speed) {
    motor_stop();
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}


// Spin the robot left in place
void spin_left(int speed) {
    motor_stop();
    // Left motor backward, Right motor forward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}


// Spin the robot right in place
void spin_right(int speed) {
    motor_stop();
    // Left motor forward, Right motor backward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}


// ===============================================================
//                         MAIN PROGRAM
// ===============================================================
int main() {
    wiringPiSetup();  // initialize WiringPi library

    // Set all motor control pins as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Initialize software PWM on both motor enable pins
    softPwmCreate(ENA, 0, 100);  // range: 0–100%
    softPwmCreate(ENB, 0, 100);

    // Variables for user input
    char cmd[10];         // stores user command
    int speed = SLOW_SPEED;  // default speed
    int running = 1;         // program loop control

    // Display command menu
    printf("=====================================\n");
    printf("       RoboRide Motor Controller     \n");
    printf("=====================================\n");
    printf("Commands:\n");
    printf(" FWD  = move forward\n");
    printf(" BCK  = move backward\n");
    printf(" LFT  = spin left\n");
    printf(" RGT  = spin right\n");
    printf(" STOP = stop motors\n");
    printf(" 1    = set slow speed (%d%%)\n", SLOW_SPEED);
    printf(" 2    = set fast speed (%d%%)\n", FAST_SPEED);
    printf(" EXIT = quit program\n");
    printf("=====================================\n");

    // Main control loop
    while (running) {
        printf("\nEnter command: ");
        scanf("%s", cmd);

        if (strcmp(cmd, "FWD") == 0) {
            forward(speed);
            printf("Moving forward at %d%% speed.\n", speed);
        }
        else if (strcmp(cmd, "BCK") == 0) {
            backward(speed);
            printf("Moving backward at %d%% speed.\n", speed);
        }
        else if (strcmp(cmd, "LFT") == 0) {
            spin_left(speed);
            printf("Spinning left at %d%% speed.\n", speed);
        }
        else if (strcmp(cmd, "RGT") == 0) {
            spin_right(speed);
            printf("Spinning right at %d%% speed.\n", speed);
        }
        else if (strcmp(cmd, "STOP") == 0) {
            motor_stop();
            printf("Motors stopped.\n");
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
            printf("Exiting program... Goodbye!\n");
        }
        else {
            printf("Invalid command! Please try again.\n");
        }
    }

    // Stop motors before closing the program
    motor_stop();
    return 0;
}
