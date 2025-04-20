// g++ -Wall -pthread -o prog *.cpp -lpigpio -lrt

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <cfloat>
#include <pigpio.h>
#include <string>
#include <fstream>
#include <streambuf>

#include <cstring>
#include <list>
#include <vector>
#include <string>
#include <iostream>

#include "globals.hpp"

using namespace std;

uint32_t startTick;

int proxSensorCount = 0;
int swingOverArmButtonCount = 0;
int startButtonCount = 0;

void interruptHandler(int x);

void waitForProxSensor();
void waitForSwingoverButton();
void waitForStartButton();
void sensorAlert(int gpio, int level, uint32_t tick, void* user);

bool soundPlaying = false;

int main() {
    gpioInitialise();

    signal(SIGCONT, SIG_IGN);
    signal(SIGINT, interruptHandler);

    //Set up start button
    gpioSetMode(START_BTN, PI_INPUT);
    gpioSetPullUpDown(START_BTN, PI_PUD_UP);
    gpioNoiseFilter(START_BTN, 5000, 0);

    //Set up proximity sensor
    gpioSetMode(PROX_SENSOR, PI_INPUT);
    gpioNoiseFilter(PROX_SENSOR, 5000, 0);

    //Set up swing-over arm button
    gpioSetMode(SWINGOVER_ARM_BTN, PI_INPUT);
    gpioSetPullUpDown(SWINGOVER_ARM_BTN, PI_PUD_UP);
    gpioNoiseFilter(SWINGOVER_ARM_BTN, 5000, 0);

    //Set Tumbler Servo to neutral position
    gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);

    //Set pendulum Servo to neutral position
    gpioServo(PENDULUM_SERVO, NEUTRAL_PULSE_WIDTH);

    // Alert function for presence sensor state transition
    gpioSetAlertFuncEx(PROX_SENSOR, sensorAlert, ((void *) "ProxSensor"));

    // Alert functions for the two buttons
    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *) "StartButton"));
    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *) "SwingOverArmButton"));

    startTick = gpioTick();

    while (true) {

        // Wait for start button
        waitForStartButton();

        // Wait for presence (wait for snap)
        waitForProxSensor();

        // Record the start time
        startTick = gpioTick();

        // Open the tumbler servo
        gpioServo(TUMBLER_SERVO, OPEN_PULSE_WIDTH);

        // Play the tumbler sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

        // Wait for the arm to hit the first button
        waitForSwingoverButton();

        // Play the stall sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/robot25/fog_horn.wav");

        // If run time is less than 90 seconds, wait the remaining time plus 5 seconds
        if ((gpioTick() - startTick) < 90000000){
            double remainingSeconds = (gpioTick() - startTick) / 1000000;
            double waitTime = remainingTime + 5;
            gpioSleep( 0, waitTime, 0);
        }

        // Open servo to release pendulums
        gpioServo(PENDULUM_SERVO, OPEN_PULSE_WIDTH);

        // Wait a bit to let people recover from their amazement
        gpioSleep( 0, 5, 0);

        // Reset Servos at the end
        gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);
        gpioServo(PENDULUM_SERVO, NEUTRAL_PULSE_WIDTH);
    }

}

void interruptHandler(int x) {
    cout << "INTERRUPT\n";

    exit(0);
}

void waitForStartButton() {
    int count = startButtonCount;

    while (count == startButtonCount) {
        usleep(10000);
    }
}

void waitForProxSensor() {
    int count = proxSensorCount;

    while (count == proxSensorCount) {
        usleep(10000);
    }
}

void waitForSwingoverButton() {
    int count = swingOverArmButtonCount;

    while (count == swingOverArmButtonCount) {
        usleep(10000);
    }
}

void sensorAlert(int gpio, int level, uint32_t tick, void* user) {

    //triggers when button pressed, not when released
    if (!(level || strcmp((const char *) user, "StartButton"))) {
        ++startButtonCount;
    }

    //triggers when button pressed, not when released
    if (!(level || strcmp((const char *) user, "SwingOverArmButton"))) {
        ++swingOverArmButtonCount;
    }

    //triggers when proximity sensor detects something
    if (level && !strcmp((const char *) user, "ProxSensor")) {
        ++proxSensorCount;
    }

    cout << (const char *) user << (!level ? "released" : "pressed ") << " at " << tick << "\n";
}

