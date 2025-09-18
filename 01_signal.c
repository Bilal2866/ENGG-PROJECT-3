/*
rm runme && gcc 01_signal.c -o runme -l wiringPi && ./runme
*/

#include <signal.h> // signal() for signal_handler
#include <stdio.h>
#include <unistd.h> // ualarm(), usleep(), nanosleep()
#include <time.h> // nanosleep()
#include <wiringPi.h> // wiringPiSetup(), pinMode(), digitalWrite()

int counter = 0;

void alarm_handler(int sig_num)
{
    // Increment counter safely within the signal handler
    counter++;
}

int main() {
    signal(SIGALRM, alarm_handler);

    /*
     The ualarm() function causes the signal SIGALRM to be sent
     - after the number of real-time microseconds by the first argument has elapsed
     - repeated timeout notifications then occur every interval microseconds if the second parameter is non-zero
    */
    ualarm(500000, 500000); // Set ualarm to trigger every 1 millisecond (1000 microseconds)

    // Define a 1 millisecond sleep interval for nanosleep
    struct timespec req, rem;
    req.tv_sec = 0;  // No seconds
    req.tv_nsec = 1000000L;  // 1 millisecond (1 million nanoseconds)

    int sleep_time_ms = 1000;  // Total sleep time in milliseconds (5 seconds)

    while(1)
    {
        // Print the current state
        printf("Inside main\n");
        printf("Counter: %d\n", counter);

        // Sleep for 5 seconds in small increments using nanosleep()
        for (int i = 0; i < sleep_time_ms; i++) {
            nanosleep(&req, &rem);  // Sleep for 1 millisecond
        }
    }

    return 0;
}
