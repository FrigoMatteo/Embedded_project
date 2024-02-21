/*
 * direction_graphics.h
 *
 *  Created on: 14 dic 2022
 *      Author: damiano
 */

#ifndef DIRECTION_GRAPHICS_H_
#define DIRECTION_GRAPHICS_H_

#include "Hardware/Graphics/menu_graphics.h"

// redraw the directions and their respective power levels
// inputs: x and y current Joystick or accelerometer values and currentModality to distinguish whether Joystick or accelerometer is being used
void drawDirections(uint64_t x, uint64_t y, Selection_t currentModality);

// redraw direction images when the current direction is FORWARD
void drawDirectionForward();

// redraw direction images when the current direction is BACKWARD
void drawDirectionBackward();

// redraw direction images when the current direction is LEFT
void drawDirectionLeft();

// redraw direction images when the current direction is RIGHT
void drawDirectionRight();

// test drawDirection() when Joystick is used
void testDirectionGraphics1();

// test drawDirection() when Accelerometer is used
void testDirectionGraphics2();

#endif /* DIRECTION_GRAPHICS_H_ */
