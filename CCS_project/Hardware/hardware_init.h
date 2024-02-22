/*
 * harware_init.h
 *
 * hardware intialization, even peripherals, headers
 *
 */

#ifndef HARDWAREINIT_H_
#define HARDWAREINIT_H_

#include "Graphics/graphics_context.h"

#define TIMER_PERIOD0 0x7530  // 30 000 to hex

// wrapper
void hwInit ();

// stops watchdog timer and initializes some flash memory features (e.g. Voltage)
void basicInit ();

// initializes timer 1 min
void timerInit ();

// initialize lightSensor
void lightSensorInit ();

// initializes the two led for rgb
void ledInit ();

// initializes Buttons on TI J4.32 and J4.33 and Joystick input button
void buttonsInit ();

//initialize Analog to Digital Converter for Joystick and enables its interrupts
void joystickInit ();

//generic initialization of ADC14, to be used in conjunction with _adcInit() and/or _joystickInit()
void adcInit ();

//initialize Liquid Crystal Display
void graphicsInit ();

#endif /* HARDWAREINIT_H_ */
