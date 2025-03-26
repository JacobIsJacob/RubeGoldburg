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

    gpioSetAlertFuncEx(BTN_GREEN, buttonAlert, ((void *) "Green"));


    startTick = gpioTick();

    while (true) {

        waitForButton();

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
