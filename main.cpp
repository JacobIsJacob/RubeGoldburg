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
int ziplineButtonCount = 0;

void interruptHandler(int x);

void waitForProxSensor();
void waitForSwingoverButton();
void waitForZiplineButton();
void sensorAlert(int gpio, int level, uint32_t tick, void* user);

bool soundPlaying = false;

int main() {
    gpioInitialise();

    signal(SIGCONT, SIG_IGN);
    signal(SIGINT, interruptHandler);

    //Set up swing-over arm button
    gpioSetMode(SWINGOVER_ARM_BTN, PI_INPUT);
    gpioSetPullUpDown(SWINGOVER_ARM_BTN, PI_PUD_UP);
    gpioNoiseFilter(SWINGOVER_ARM_BTN, 5000, 0);

    //Set up zipline button
    gpioSetMode(ZIPLINE_BTN, PI_INPUT);
    gpioSetPullUpDown(ZIPLINE_BTN, PI_PUD_UP);
    gpioNoiseFilter(ZIPLINE_BTN, 5000, 0);

    //Set up proximity sensor
    gpioSetMode(PROX_SENSOR, PI_INPUT);
    gpioNoiseFilter(PROX_SENSOR, 5000, 0);

    //Set Tumbler Servo to neutral position
    gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);

    //Set pendulum Servo to neutral position
    gpioServo(PENDULUM_SERVO, NEUTRAL_PULSE_WIDTH);

    // Various other GPIO setups for button, fan, presence sensor, etc

    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *) "SwingOverArmButton"));
    
    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *) "ZiplineButton"));

    // Alert function for presence sensor state transition

    gpioSetAlertFuncEx(PROX_SENSOR, sensorAlert, ((void *) "ProxSensor"));

    startTick = gpioTick();

    while (true) {

        // waitForPresence() (wait for snap)

        // trigger the tumbler servo
        gpioServo(TUMBLER_SERVO, OPEN_PULSE_WIDTH);

        // play the "start" sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/robot25/fog_horn.wav");

        waitForProxSensor();

        waitForSwingoverButton();

        // turn on the fan
        // probably sleep for X seconds with fan turned on
        // turn off the fan

        // wait for zipline button

        waitForZiplineButton();

        // use servo to release pendulum
        gpioServo(PENDULUM_SERVO, OPEN_PULSE_WIDTH);

        // play some other sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/robot25/fog_horn.wav");
    }

}

void interruptHandler(int x) {
    cout << "INTERRUPT\n";

    exit(0);
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

void waitForZiplineButton() {
    int count = ziplineButtonCount;

    while (count == ziplineButtonCount) {
        usleep(10000);
    }
}

void sensorAlert(int gpio, int level, uint32_t tick, void* user) {

    //triggers when button pressed, not when released
    if (!(level || strcmp((const char *) user, "SwingOverArmButton"))) {
        ++swingOverArmButtonCount;
    }

    //triggers when button pressed, not when released
    if (!(level || strcmp((const char *) user, "ZiplineButton"))) {
        ++ziplineButtonCount;
    }

    //triggers when proximity sensor detects something
    if (level && !strcmp((const char *) user, "ProxSensor") && proxSensorCount < 1) {
        ++proxSensorCount;
    }

    cout << (const char *) user << (!level ? "released" : "pressed ") << " at " << tick << "\n";
}

