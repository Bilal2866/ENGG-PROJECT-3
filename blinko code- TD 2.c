#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <wiringPi.h>

#define LED_PIN 0          // WiringPi pin number (GPIO17)
#define PERIOD_MS 1000     // 1 second = 1000 ticks
#define TICK_US 1000       // 1 ms = 1000 microseconds

volatile int tick = 0;
volatile int duty = 0;     // Duty cycle in ms (0–1000)

// ---- Signal handler ----
void pwm_handler(int sig) {
    tick = (tick + 1) % PERIOD_MS;
    if (tick < duty) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
}

// ---- Non-blocking keyboard setup ----
void set_nonblocking(int enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        ttystate.c_lflag |= ICANON | ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
        fcntl(STDIN_FILENO, F_SETFL, 0);
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "WiringPi setup failed\n");
        return 1;
    }

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    signal(SIGALRM, pwm_handler);
    ualarm(TICK_US, TICK_US);  // 1 ms interval

    set_nonblocking(1);

    printf("Controls: a=25%%  s=50%%  d=75%%  w=100%%  q=off  x=exit\n");

    int running = 1;
    while (running) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            switch (c) {
                case 'a': duty = 250; printf("Duty = 25%%\n"); break;
                case 's': duty = 500; printf("Duty = 50%%\n"); break;
                case 'd': duty = 750; printf("Duty = 75%%\n"); break;
                case 'w': duty = 1000; printf("Duty = 100%%\n"); break;
                case 'q': duty = 0; printf("LED off\n"); break;
                case 'x': running = 0; break;
                case '\n': break; // ignore enter key
                default: printf("Invalid input\n"); break;
            }
            fflush(stdout);
        }
        usleep(5000); // small delay
    }

    set_nonblocking(0);
    digitalWrite(LED_PIN, LOW);
    printf("Exiting program.\n");
    return 0;
}
