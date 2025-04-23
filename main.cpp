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
void sensorAlert(int gpio, int level, uint32_t tick, void *user);
void playSoundWithTimeout(const char *path, int timeoutSeconds);

bool soundPlaying = false;

int main()
{
    gpioInitialise();

    signal(SIGCONT, SIG_IGN);
    signal(SIGINT, interruptHandler);

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
    gpioSetMode(FIRST_AVENGER, PI_OUTPUT);
    gpioWrite(FIRST_AVENGER, 0);

    gpioSetMode(SECOND_AVENGER, PI_OUTPUT);
    gpioWrite(SECOND_AVENGER, 0);

    gpioSetMode(THIRD_AVENGER, PI_OUTPUT);
    gpioWrite(THIRD_AVENGER, 0);

    gpioSetMode(FOURTH_AVENGER, PI_OUTPUT);
    gpioWrite(FOURTH_AVENGER, 0);

    // Set Tumbler Servo to neutral position
    gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);

    // Set pendulum Servo to neutral position
    gpioServo(PENDULUM_SERVO, NEUTRAL_PULSE_WIDTH);

    // Alert function for presence sensor state transition
    gpioSetAlertFuncEx(PROX_SENSOR, sensorAlert, ((void *)"ProxSensor"));

    // Alert functions for the two buttons
    gpioSetAlertFuncEx(SWINGOVER_ARM_BTN, sensorAlert, ((void *)"StartButton"));
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

        // Open the tumbler servo
        gpioServo(TUMBLER_SERVO, OPEN_PULSE_WIDTH);

        // Play the tumbler sound
        cout << "Play Sound";
        void playSoundWithTimeout("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav", 10);

        // Wait for the arm to hit the first button
        waitForSwingoverButton();

        // If run time is less than 90 seconds, play music for the remaining time plus 5 seconds
        if ((gpioTick() - startTick) < 90000000)
        {
            double remainingSeconds = (gpioTick() - startTick) / 1000000;
            int waitTime = (int)round(remainingTime + 5);

            // Play the background sound
            cout << "Play Background Sound";
            void playSoundWithTimeout("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav", waitTime);
        }

        // While run time is less than 95 seconds, play taglines
        while ((gpioTick() - startTick) < 90000000)
        {
            // light up first light, play sound, then turn off the light
            gpioWrite(FIRST_AVENGER, 1);
            cout << "Play First Avenger Sound";
            system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

            gpioWrite(FIRST_AVENGER, 0);

            // Light up second light, play sound, then turn off the light
            gpioWrite(SECOND_AVENGER, 1);
            cout << "Play Second Avenger Sound";
            system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

            gpioWrite(SECOND_AVENGER, 0);

            // Light up third light, play sound, then turn off the light
            gpioWrite(THIRD_AVENGER, 1);
            cout << "Play Third Avenger Sound";
            system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

            gpioWrite(THIRD_AVENGER, 0);

            // Light up fourth light, play sound, then turn off the light
            gpioWrite(FOURTH_AVENGER, 1);
            cout << "Play Fourth Avenger Sound";
            system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

            gpioWrite(FOURTH_AVENGER, 0);
        }

        // Play final sound
        cout << "Play Fourth Avenger Sound";
        system("/usr/bin/aplay /home/jacob/RubeGoldberg/john_cena.wav");

        // Turn on all lights
        gpioWrite(FIRST_AVENGER, 1);
        gpioWrite(SECOND_AVENGER, 1);
        gpioWrite(THIRD_AVENGER, 1);
        gpioWrite(FOURTH_AVENGER, 1);

        // Open servo to release pendulums
        gpioServo(PENDULUM_SERVO, OPEN_PULSE_WIDTH);

        // Turn off all lights after waiting 2 seconds
        gpioSleep(0, 2, 0);

        gpioWrite(FIRST_AVENGER, 0);
        gpioWrite(SECOND_AVENGER, 0);
        gpioWrite(THIRD_AVENGER, 0);
        gpioWrite(FOURTH_AVENGER, 0);

        // Wait a bit to let people recover from their amazement
        gpioSleep(0, 2, 0);

        // Reset Servos at the end
        gpioServo(TUMBLER_SERVO, NEUTRAL_PULSE_WIDTH);
        gpioServo(PENDULUM_SERVO, NEUTRAL_PULSE_WIDTH);
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
    if (!(level || strcmp((const char *)user, "StartButton")))
    {
        ++startButtonCount;
    }

    // triggers when button pressed, not when released
    if (!(level || strcmp((const char *)user, "SwingOverArmButton")))
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

void playSoundWithTimeout(const char *path, int timeoutSeconds)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        // Child process: play the sound
        std::string command = "/usr/bin/aplay ";
        command += path;
        system(command.c_str());
        _exit(0); // Use _exit to avoid flushing shared stdio buffers
    }
    else
    {
        // Parent: wait with timeout
        int status;
        int waited = 0;

        while (waited < timeoutSeconds)
        {
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == 0)
            {
                sleep(1);
                waited++;
            }
            else if (result == pid)
            {
                // Child finished
                return;
            }
            else
            {
                perror("waitpid error");
                return;
            }
        }

        // Timeout reached: kill child
        std::cout << "Timeout reached. Killing sound process." << std::endl;
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0); // Ensure cleanup
    }
}
