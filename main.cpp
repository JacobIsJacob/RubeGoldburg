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
#include <sys/types.h>
#include <sys/wait.h>

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
double remainingSeconds = 0;

void interruptHandler(int x);

void waitForProxSensor();
void waitForSwingoverButton();
void waitForStartButton();
void sensorAlert(int gpio, int level, uint32_t tick, void *user);
pid_t playSoundInBackground(const char *path);

bool soundPlaying = false;

int main()
{
    gpioInitialise();

    signal(SIGCONT, SIG_IGN);
    signal(SIGINT, interruptHandler);
    signal(SIGCHLD, SIG_IGN);

    // Set up start button
    gpioSetMode(START_BTN, PI_INPUT);
    gpioSetPullUpDown(START_BTN, PI_PUD_UP);
    gpioNoiseFilter(START_BTN, 5000, 0);

    // Set up proximity sensor
    gpioSetMode(PROX_SENSOR, PI_INPUT);
    gpioNoiseFilter(PROX_SENSOR, 5000, 0);

    // Set up swing-over arm button
    gpioSetMode(SWINGOVER_ARM_BTN, PI_INPUT);
    gpioSetPullUpDown(SWINGOVER_ARM_BTN, PI_PUD_UP);
    gpioNoiseFilter(SWINGOVER_ARM_BTN, 5000, 0);

    // Set up avenger lights
    gpioSetMode(IRONMAN, PI_OUTPUT);
    gpioWrite(IRONMAN, 0);

    gpioSetMode(HULK, PI_OUTPUT);
    gpioWrite(HULK, 0);

    gpioSetMode(THOR, PI_OUTPUT);
    gpioWrite(THOR, 0);

    gpioSetMode(CAPTN_AMERICA, PI_OUTPUT);
    gpioWrite(CAPTN_AMERICA, 0);

    // Set Tumbler Servo to neutral position
    gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);

    // Set pendulum Servo to CLOSED position
    gpioServo(PENDULUM_SERVO, CLOSED_PULSE_WIDTH);

    // Alert function for presence sensor state transition
    gpioSetAlertFuncEx(PROX_SENSOR, sensorAlert, ((void *)"ProxSensor"));

    // Alert functions for the two buttons
    gpioSetAlertFuncEx(START_BTN, sensorAlert, ((void *)"StartButton"));
    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *)"SwingOverArmButton"));

    startTick = gpioTick();

    while (true)
    {

        // Wait for start button
        waitForStartButton();

        // Wait for presence (wait for snap)
        waitForProxSensor();

        // Record the start time
        startTick = gpioTick();

        // Wait for snap
        gpioSleep(0, 1, 0);

        // Play the tumbler sound
        cout << "Play Wongy Sound";
        pid_t wongSoundPid = playSoundInBackground("/home/jacob/RubeGoldburg/Wong_infinity_stones.wav");

        gpioSleep(0, 7, 0);

        // Open the tumbler servo
        gpioServo(TUMBLER_SERVO, CLOSED_PULSE_WIDTH);

        // Wait for the arm to hit the first button
        waitForSwingoverButton();

        // Stop background sound
        kill(wongSoundPid, SIGKILL);

        // Play Shpooder-Man sounds
        cout << "Play Groan Sound";
        system("/usr/bin/aplay /home/jacob/RubeGoldburg/groan.wav");

        cout << "Play Spiderman We're the Avengers Sound";
        system("/usr/bin/aplay /home/jacob/RubeGoldburg/Spiderman_Avengers.wav");

        // Play the background sound
        cout << "Play Background Sound";
        pid_t soundPid = playSoundInBackground("/home/jacob/RubeGoldburg/Avengers_theme.wav");

        // Wait a bit before playing first tagline
        gpioSleep(0, 2, 0);

        // light up first light, play sound, then turn off the light
        gpioWrite(IRONMAN, 1);
        cout << "Play IRONMAN Sound \n";
        system("/usr/bin/aplay /home/jacob/RubeGoldburg/Iron_Man.wav");
        gpioWrite(IRONMAN, 0);

        // Give a bit of rest between Taglines
        gpioSleep(0, 2, 0);

        // Light up second light, play sound, then turn off the light
        gpioWrite(HULK, 1);
        cout << "Play HULK Sound \n";
        system("/usr/bin/aplay /home/jacob/RubeGoldburg/Hulk_smash.wav");
        gpioWrite(HULK, 0);

        // Give a bit of rest between Taglines
        gpioSleep(0, 1, 0);

        // Light up third light, play sound, then turn off the light
        gpioWrite(THOR, 1);
        cout << "Play THOR Sound \n";
        system("/usr/bin/aplay /home/jacob/RubeGoldburg/Thor_another.wav");
        gpioWrite(THOR, 0);

        // Give a bit of rest between Taglines
        gpioSleep(0, 1, 0);

        // Stop background sound
        kill(soundPid, SIGKILL);

        // Light up fourth light, play sound, but leave the light on
        gpioWrite(CAPTN_AMERICA, 1);
        cout << "Play CAPTN_AMERICA Sound \n";
        playSoundInBackground("/home/jacob/RubeGoldburg/Avengers_Assemble.wav");

        // Let the the Captain cook
        gpioSleep(0, 8, 0);

        // Alternate lights
        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 1);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 1);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioSleep(0,1,0);

        gpioWrite(CAPTN_AMERICA, 0);
        gpioWrite(IRONMAN, 1);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 1);
        gpioSleep(0,1,0);

        // Open servo to release pendulums
        gpioServo(PENDULUM_SERVO, OPEN_PULSE_WIDTH);

        // Turn off all lights after waiting 1 second
        gpioSleep(0, 2, 0);

        gpioWrite(IRONMAN, 0);
        gpioWrite(HULK, 0);
        gpioWrite(THOR, 0);
        gpioWrite(CAPTN_AMERICA, 0);

        // Let people recover from their amazement
        gpioSleep(0, 2, 0);

        // Reset Servos at the end
        gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);
        gpioServo(PENDULUM_SERVO, CLOSED_PULSE_WIDTH);
    }
}

void interruptHandler(int x)
{
    cout << "INTERRUPT\n";

    exit(0);
}

void waitForStartButton()
{
    int count = startButtonCount;

    while (count == startButtonCount)
    {
        usleep(10000);
    }
}

void waitForProxSensor()
{
    int count = proxSensorCount;

    while (count == proxSensorCount)
    {
        usleep(10000);
    }
}

void waitForSwingoverButton()
{
    int count = swingOverArmButtonCount;

    while (count == swingOverArmButtonCount)
    {
        usleep(10000);
    }
}

void sensorAlert(int gpio, int level, uint32_t tick, void *user)
{

    // triggers when button pressed, not when released
    if (level && !strcmp((const char *)user, "StartButton"))
    {
        ++startButtonCount;
    }

    // triggers when button pressed, not when released
    if (level && !strcmp((const char *)user, "SwingOverArmButton"))
    {
        ++swingOverArmButtonCount;
    }

    // triggers when proximity sensor detects something
    if (level && !strcmp((const char *)user, "ProxSensor"))
    {
        ++proxSensorCount;
    }

    cout << (const char *)user << (!level ? "released" : "pressed ") << " at " << tick << "\n";
}

pid_t playSoundInBackground(const char *path)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return -1;
    }

    if (pid == 0)
    {
        // Child process: replace with aplay directly
        execlp("aplay", "aplay", path, (char *)nullptr);

        // If execlp fails
        perror("execlp failed");
        _exit(1);
    }

    // Parent process: return child's PID
    return pid;
}