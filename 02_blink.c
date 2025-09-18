/*
gcc 02_blink.c -o runme -l wiringPi
*/

#include <wiringPi.h>
#include <stdio.h>

const int ledPin = 0;  // GPIO pin 11

int main() {

    wiringPiSetup();

    pinMode(ledPin, OUTPUT);

    // infinite while loop
    while(1){

        printf("Turn LED On \n");
        digitalWrite(ledPin, HIGH);

        delay(1000);
 
        printf("Turn LED Off \n");
        digitalWrite(ledPin, LOW);

        delay(1000);
    }    

    return 0;

}