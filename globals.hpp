#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/*
 * Not sure where these PWM pins are used (maybe speaker?),
 * left them there just in case, but need to use two separate
 * PWM signals for the servos.
 */
#define LEFT_PWM 12

#define TUMBLER_SERVO 12

#define RIGHT_PWM 13

#define PENDULUM_SERVO 13

#define SWINGOVER_ARM_BTN 17

#define ZIPLINE_BTN 22

#define PROX_SENSOR 27

#define NEUTRAL_PULSE_WIDTH 1500

#define OPEN_PULSE_WIDTH 2500

extern uint32_t startTick;

#endif
