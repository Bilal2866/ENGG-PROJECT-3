#include <wiringPi.h>
#include <softPwm.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================= PIN MAPPING =========================
//
// Motor A (Left)
//   IN1 → WiringPi 0 (BCM17)
//   IN2 → WiringPi 1 (BCM18)
//   ENA → WiringPi 2 (BCM27)
//
// Motor B (Right)
//   IN3 → WiringPi 3 (BCM22)
//   IN4 → WiringPi 4 (BCM23)
//   ENB → WiringPi 5 (BCM24)
//
// Ultrasonic (Mounted at BACK of car)
//   TRIG → WiringPi 21 (BCM5)
//   ECHO → WiringPi 26 (BCM12) — through 1k/2k divider
//
// Camera Module 2
//   Connected to CSI camera port on Pi Zero W
//
// ===============================================================

// Motor pins
#define IN1 0
#define IN2 1
#define ENA 2
#define IN3 3
#define IN4 4
#define ENB 5

// Ultrasonic pins
#define TRIG 21
#define ECHO 26

// Speed presets
#define SLOW_SPEED 70
#define FAST_SPEED 90

// Motion states
#define STOPPED     0
#define FORWARD_M   1
#define BACKWARD_M  2
#define LEFT_M      3
#define RIGHT_M     4

// Auto-stop threshold (cm) for reverse
#define REVERSE_STOP_DISTANCE_CM 20.0f

int motion_state = STOPPED;

// ================= MOTOR CONTROL =================

void motor_stop(void) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    softPwmWrite(ENA, 0);
    softPwmWrite(ENB, 0);
    delay(100); // small safety delay
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

// ================= ULTRASONIC FUNCTION =================

float getDistance(void) {
    // Ensure TRIG low
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    // 10 µs trigger pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    // Wait for echo start
    unsigned long start = micros();
    while (digitalRead(ECHO) == LOW) {
        if (micros() - start > 250000) { // 250 ms timeout
            return -1.0f;
        }
    }

    unsigned long echo_start = micros();

    // Wait for echo end
    while (digitalRead(ECHO) == HIGH) {
        if (micros() - echo_start > 250000) { // 250 ms timeout
            return -1.0f;
        }
    }

    unsigned long echo_end = micros();
    float distance = (echo_end - echo_start) / 58.0f; // HC-SR04 formula (cm)

    if (distance < 2.0f || distance > 400.0f) {
        return -1.0f;
    }

    return distance;
}

// ================= CAMERA HELPERS =================

// Ensure ~/Pictures and ~/Videos exist
void ensure_media_dirs(void) {
    const char *home = getenv("HOME");
    if (!home) home = "/home/pi";

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "mkdir -p '%s/Pictures' '%s/Videos'",
             home, home);
    system(cmd);
}

// Build timestamped path: dir/prefix_YYYY-MM-DD_HH-MM-SS.ext
void make_timestamp_path(char *out, size_t size,
                         const char *subdir,
                         const char *prefix,
                         const char *ext) {
    const char *home = getenv("HOME");
    if (!home) home = "/home/pi";

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d_%H-%M-%S", t);

    snprintf(out, size, "%s/%s/%s_%s.%s",
             home, subdir, prefix, timestr, ext);
}

// Stop robot, then take a photo using libcamera-still (no preview)
void capture_photo(void) {
    motor_stop();
    motion_state = STOPPED;

    ensure_media_dirs();

    char path[512];
    make_timestamp_path(path, sizeof(path), "Pictures", "photo", "jpg");

    char cmd[768];
    // -n = no preview (works over SSH), redirect output to /dev/null
    snprintf(cmd, sizeof(cmd),
             "libcamera-still -n -o '%s' >/dev/null 2>&1",
             path);

    mvprintw(18, 0, "Camera: Capturing photo...                      ");
    refresh();
    system(cmd);
    mvprintw(18, 0, "Camera: Photo saved to %s                       ", path);
}

// Stop robot, then start video recording (no preview, indefinite until stopped)
void start_video(void) {
    motor_stop();
    motion_state = STOPPED;

    ensure_media_dirs();

    char path[512];
    make_timestamp_path(path, sizeof(path), "Videos", "video", "h264");

    char cmd[768];
    // -n = no preview, -t 0 = record until stopped, & = background
    snprintf(cmd, sizeof(cmd),
             "libcamera-vid -n -t 0 -o '%s' >/dev/null 2>&1 &",
             path);

    mvprintw(19, 0, "Camera: Starting video recording...             ");
    refresh();
    system(cmd);
    mvprintw(19, 0, "Camera: Recording video to %s                   ", path);
}

// Stop video recording
void stop_video(void) {
    motor_stop();
    motion_state = STOPPED;

    mvprintw(19, 0, "Camera: Stopping video recording...             ");
    refresh();
    // Kill any running libcamera-vid process
    system("pkill libcamera-vid >/dev/null 2>&1");
    mvprintw(19, 0, "Camera: Video recording stopped                 ");
}

// ================= MAIN =================

int main(void) {
    if (wiringPiSetup() == -1) {
        printf("ERROR: wiringPi setup failed\n");
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

    // Ncurses setup
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);  // getch() non-blocking
    keypad(stdscr, TRUE);

    // UI: Title and static text
    mvprintw(0, 0,  "=======================================================");
    mvprintw(1, 0,  "                       RoboRide                        ");
    mvprintw(2, 0,  "                     Drive and Capture                 ");
    mvprintw(3, 0,  "=======================================================");

    mvprintw(5, 0,  "Controls:");
    mvprintw(6, 0,  "  W = Forward      S = Backward");
    mvprintw(7, 0,  "  A = Left         D = Right");
    mvprintw(8, 0,  "  1 = Slow Speed   2 = Fast Speed");
    mvprintw(9, 0,  "  SPACE = Stop     Q = Quit");
    mvprintw(10, 0, "Camera:");
    mvprintw(11, 0, "  C = Capture Photo");
    mvprintw(12, 0, "  V = Start Video Recording");
    mvprintw(13, 0, "  B = Stop Video Recording");

    mvprintw(15, 0, "Status:");
    mvprintw(16, 0, "  Motion: STOPPED");
    mvprintw(17, 0, "  Speed Mode: SLOW");
    mvprintw(18, 0, "Camera: Ready");
    mvprintw(19, 0, "Camera: (no video recording in progress)");
    mvprintw(21, 0, "Sensors:");
    mvprintw(22, 0, "  Distance: ---");
    mvprintw(23, 0, "  Reverse Obstacle: ---");

    refresh();

    int speed = SLOW_SPEED;
    int ch;
    unsigned long last_dist_ms = 0;
    float last_distance = -1.0f;

    while (1) {
        // -------- Handle keyboard input --------
        ch = getch();
        if (ch != ERR) {
            switch (ch) {
                case 'w': case 'W':
                    forward(speed);
                    motion_state = FORWARD_M;
                    mvprintw(16, 0, "  Motion: FORWARD                        ");
                    break;

                case 's': case 'S':
                    backward(speed);
                    motion_state = BACKWARD_M;
                    mvprintw(16, 0, "  Motion: BACKWARD                       ");
                    break;

                case 'a': case 'A':
                    spin_left(speed);
                    motion_state = LEFT_M;
                    mvprintw(16, 0, "  Motion: TURN LEFT                      ");
                    break;

                case 'd': case 'D':
                    spin_right(speed);
                    motion_state = RIGHT_M;
                    mvprintw(16, 0, "  Motion: TURN RIGHT                     ");
                    break;

                case ' ':
                    motor_stop();
                    motion_state = STOPPED;
                    mvprintw(16, 0, "  Motion: STOPPED                        ");
                    break;

                case '1':
                    speed = SLOW_SPEED;
                    mvprintw(17, 0, "  Speed Mode: SLOW                       ");
                    break;

                case '2':
                    speed = FAST_SPEED;
                    mvprintw(17, 0, "  Speed Mode: FAST                       ");
                    break;

                // Camera controls
                case 'c': case 'C':
                    capture_photo();
                    break;

                case 'v': case 'V':
                    start_video();
                    break;

                case 'b': case 'B':
                    stop_video();
                    break;

                case 'q': case 'Q':
                    motor_stop();
                    stop_video(); // just in case
                    endwin();
                    return 0;

                default:
                    break;
            }
        }

        // -------- Ultrasonic reading every 2 seconds --------
        unsigned long now = millis();
        if (now - last_dist_ms > 2000) {
            last_dist_ms = now;

            float d = getDistance();
            last_distance = d;

            if (d < 0) {
                mvprintw(22, 0, "  Distance: --- (no echo)                ");
                mvprintw(23, 0, "  Reverse Obstacle: UNKNOWN              ");
            } else {
                mvprintw(22, 0, "  Distance: %6.2f cm                     ", d);

                // Auto-stop only when reversing and obstacle is too close
                if (motion_state == BACKWARD_M && d < REVERSE_STOP_DISTANCE_CM) {
                    motor_stop();
                    motion_state = STOPPED;
                    mvprintw(16, 0, "  Motion: STOPPED (AUTO REVERSE STOP)    ");
                    mvprintw(23, 0, "  Reverse Obstacle: TOO CLOSE!           ");
                } else {
                    mvprintw(23, 0, "  Reverse Obstacle: CLEAR                ");
                }
            }
        }

        refresh();
        delay(20); // avoid maxing CPU
    }

    endwin();
    return 0;
}
