/*
 * direction_graphics.h
 *
 *  Created on: 14 dic 2022
 *      Author: damiano
 */

#ifndef MENU_GRAPHICS_H_
#define MENU_GRAPHICS_H_

#include <stdint.h>

#include <Hardware/status.h>
extern State_mqtt_t current_state_mqtt;

extern uint16_t mqttBuffer[2][5];  // h, min, day, month, year, data
extern uint16_t sensorsBuffer[];

extern bool alarm_led;
extern bool alarm_mqtt;
extern bool alarm_active;

void draw_current_menu ();
void draw_mqtt_menu ();
void draw_sensors_menu ();
void draw_alarm_menu ();

void newBackGround ();

// DRAW COMPLETE TABLE + SELECTION
void table_draw ();

// DRAW TABLE CELLS
void draw_cells ();

// COLOR CELLS
void color_cells ();

// DRAW LETTERS
void draw_letters ();

// DRAW SEL CELL
void draw_sel_cell ();

void alarm_screen ();

#endif /* MENU_GRAPHICS_H_ */
