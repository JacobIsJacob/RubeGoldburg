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

int greenButtonCount = 0;

void interruptHandler(int x);

void waitForButton();
void buttonAlert(int gpio, int level, uint32_t tick, void* user);

bool soundPlaying = false;

int main() {
    gpioInitialise();

    signal(SIGCONT, SIG_IGN);
    signal(SIGINT, interruptHandler);

    gpioSetMode(BTN_GREEN, PI_INPUT);
    gpioSetPullUpDown(BTN_GREEN, PI_PUD_UP);
    gpioNoiseFilter(BTN_GREEN, 5000, 0);

    // Various other GPIO setups for button, fan, presence sensor, etc

    gpioSetAlertFuncEx(BTN_GREEN, buttonAlert, ((void *) "Green"));

    // Alert function for presense sensor state transition

    startTick = gpioTick();

    while (true) {

        // waitForPresence() (wait for snap)

        // trigger the tumbler servo (gpioWrite)

        // play the "start" sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/robot25/fog_horn.wav");


        waitForButton();

        // turn on the fan
        // probably sleep for X seconds with fan turned on
        // turn off the fan

        // wait for 2nd button
        // use servo to release pendulum
        
        // play some other sound
        cout << "Play Sound";
        system("/usr/bin/aplay /home/jacob/robot25/fog_horn.wav");
    }

}

void interruptHandler(int x) {
    cout << "INTERRUPT\n";

    exit(0);
}


void waitForButton() {
    int count = greenButtonCount;

    while (count == greenButtonCount) {
        usleep(10000);
    }
}

void buttonAlert(int gpio, int level, uint32_t tick, void* user) {
    if (level && !strcmp((const char *) user, "Green")) {
        ++greenButtonCount;
    }
   
    cout << (const char *) user << " button " << (!level ? "released" : "pressed ") << " at " << tick << "\n";
}
