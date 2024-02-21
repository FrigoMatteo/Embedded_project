/*
 * menu_graphics.c
 *
 *  Created on: 17/02/2024
 *      Author: kenny
 */

#include <Hardware/Graphics/images_definitions.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

#include <stdio.h>

#include "Hardware/LcdDriver/Crystalfontz128x128_ST7735.h"

#include "Hardware/Graphics/menu_graphics.h"
#include "Hardware/Graphics/images.h"
#include "Hardware/Graphics/graphics_context.h"

// function define ( CALL ME MADNESS )
#define ifState_Color_or_Black(curr, state) (curr == state) ? Graphics_setForegroundColor (&g_sContext, BACKGROUNG_COLOR) : Graphics_setForegroundColor (&g_sContext, GRAPHICS_COLOR_BLACK)

#define drawCell(cell, offset) Graphics_drawImage(&g_sContext, &cell, offset_x_table + 1 + offset_interaction * offset, offset_y_table + 1);
#define drawRectangle(offset) Graphics_drawRectangle(&g_sContext, &(Graphics_Rectangle){offset_x_sel + offset_interaction * offset, offset_y_sel, offset_x_sel + dim_sel_rectangle + offset_interaction * offset, offset_y_sel + dim_sel_rectangle});

#define drawStringCentered_witOffset(string, offset) Graphics_drawStringCentered(&g_sContext, (int8_t *)string,  AUTO_STRING_LENGTH, offset_x_text + offset_interaction *offset, offset_y_text, TRANSPARENT_TEXT)

#define drawStringCentered_xyOffset(string, offset_x, offset_y) Graphics_drawStringCentered(&g_sContext, (int8_t *)string,  AUTO_STRING_LENGTH, offset_x, offset_y, OPAQUE_TEXT)
#define drawStringCentered_Alarm(string, offset_y) drawStringCentered_xyOffset(string, offset_x_alarm_text, offset_y)
#define drawStringMid(string, offset_y) drawStringCentered_xyOffset (string, offset_x_mid, offset_y)
#define drawTitle(title) drawStringMid(title, offset_y_title)


// OFFSETS
#define offset_x_mid 65
#define offset_y_title 15
#define offset_y_subtitle 30

// table
#define offset_x_table 21
#define offset_y_table 95
#define offset_interaction 22
#define offset_x_sel offset_x_table + 2
#define offset_y_sel offset_y_table + 2
#define dim_sel_rectangle 18

// mqtt
const char status_mqtt[3][20] = {"status : CONNECTED", "status : OFFLINE", "status : ERROR"};
const char MONTH[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#define offset_y_mqtt_2 45
#define offset_y_mqtt_3 60
#define offset_y_mqtt_4 72
#define offset_y_mqtt_5 85

#define no_date "  --       --  "

// sensors
#define offset_y_sensors_temperature 40
#define offset_y_sensors_lux 60
#define offset_y_sensors_humidity 80

// alarm
#define offset_x_alarm_text 45
#define offset_y_alarm_text 39
#define inter_text 24
#define offset_x_alarm_label 80
#define offset_y_alarm_label 30

// vertical line
#define start_y_vline_a 25
#define start_y_vline_b 40
#define end_y_vline 90

// ARROWS
const Graphics_Image ARROW_1BPP_UNCOMP[16] = {
    // WHITE
   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_WHITE_BLACK_1BPP_UNCOMP, pixel_UP_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_WHITE_BLACK_1BPP_UNCOMP, pixel_SX_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_WHITE_BLACK_1BPP_UNCOMP, pixel_DOWN_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_WHITE_BLACK_1BPP_UNCOMP, pixel_DX_1BPP_UNCOMP},
    // GREEN
   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_GREEN_BLACK_1BPP_UNCOMP, pixel_UP_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_GREEN_BLACK_1BPP_UNCOMP, pixel_SX_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_GREEN_BLACK_1BPP_UNCOMP, pixel_DOWN_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_GREEN_BLACK_1BPP_UNCOMP, pixel_DX_1BPP_UNCOMP},
    // BLUE
   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_BLUE_BLACK_1BPP_UNCOMP, pixel_UP_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_BLUE_BLACK_1BPP_UNCOMP, pixel_SX_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_BLUE_BLACK_1BPP_UNCOMP, pixel_DOWN_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_BLUE_BLACK_1BPP_UNCOMP, pixel_DX_1BPP_UNCOMP},
    // PURPLE
   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_PURPLE_BLACK_1BPP_UNCOMP, pixel_UP_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_PURPLE_BLACK_1BPP_UNCOMP, pixel_SX_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_width, arrow_height, arrow_size,
    palette_PURPLE_BLACK_1BPP_UNCOMP, pixel_DOWN_1BPP_UNCOMP},

   {IMAGE_FMT_1BPP_UNCOMP, arrow_height, arrow_width, arrow_size,
    palette_PURPLE_BLACK_1BPP_UNCOMP, pixel_DX_1BPP_UNCOMP},
};

// LETTERS FOR TIMEFRAME

const Graphics_Image BLACK_CELL_1BPP_UNCOMP ={
    IMAGE_FMT_1BPP_UNCOMP,
    cell_width,
    cell_height,
    cell_size,
    palette_BLACK_1BPP_UNCOMP,
    pixel_CELL_1BPP_UNCOMP
};

const Graphics_Image CELL_1BPP_UNCOMP[4] = {
    // WHITE
   {IMAGE_FMT_1BPP_UNCOMP,
    cell_width,
    cell_height,
    cell_size,
    palette_WHITE_1BPP_UNCOMP,
    pixel_CELL_1BPP_UNCOMP},
    // GREEN
   {IMAGE_FMT_1BPP_UNCOMP,
    cell_width,
    cell_height,
    cell_size,
    palette_GREEN_1BPP_UNCOMP,
    pixel_CELL_1BPP_UNCOMP},
    // BLUE
   {IMAGE_FMT_1BPP_UNCOMP,
    cell_width,
    cell_height,
    cell_size,
    palette_BLUE_1BPP_UNCOMP,
    pixel_CELL_1BPP_UNCOMP},
    // PURPLE
   {IMAGE_FMT_1BPP_UNCOMP,
    cell_width,
    cell_height,
    cell_size,
    palette_PURPLE_1BPP_UNCOMP,
    pixel_CELL_1BPP_UNCOMP},
};

void draw_cells () {
    int i;
    for (i = 0; i < 4; i++) {
        Graphics_Rectangle cell = {
            offset_x_table + i * offset_interaction,
            offset_y_table,
            offset_x_table + (i+1) * offset_interaction,
            offset_y_table + offset_interaction,
        };
        Graphics_drawRectangle(&g_sContext, &cell);
    }
}

void color_cells () {
    int select_switch = (current_state == LEFT_STATE) ? curr_tf_mqtt : curr_tf;

    drawCell (WHITE_CELL, 0);
    drawCell (WHITE_CELL, 1);
    drawCell (WHITE_CELL, 2);
    drawCell (WHITE_CELL, 3);

    drawCell (BLACK_CELL, select_switch);
}

void draw_letters () {
    int offset_x_text = offset_x_table + offset_interaction /2 + 1;
    int offset_y_text = offset_y_table + offset_interaction /2;

    if (current_state == LEFT_STATE) {

        ifState_Color_or_Black(curr_tf_mqtt, DD);
        drawStringCentered_witOffset("DD", 0);

        ifState_Color_or_Black(curr_tf_mqtt, MM);
        drawStringCentered_witOffset("MM", 1);

        ifState_Color_or_Black(curr_tf_mqtt, YY);
        drawStringCentered_witOffset("YY", 2);

        ifState_Color_or_Black(curr_tf_mqtt, AT);
        drawStringCentered_witOffset("AT", 3);
    } else {
        ifState_Color_or_Black(curr_tf, M_15);
        drawStringCentered_witOffset("15", 0);

        ifState_Color_or_Black(curr_tf, M_30);
        drawStringCentered_witOffset("30", 1);

        ifState_Color_or_Black(curr_tf, H_1);
        drawStringCentered_witOffset("60", 2);

        ifState_Color_or_Black(curr_tf, D_1);
        drawStringCentered_witOffset("DD", 3);
    }
    Graphics_setForegroundColor (&g_sContext, GRAPHICS_COLOR_BLACK);
}

void draw_sel_cell () {
    int select_switch = (current_state == LEFT_STATE) ? sel_tf_mqtt : sel_tf;
    drawRectangle(select_switch);
}

void table_draw () {
    draw_cells ();
    color_cells ();
    draw_letters ();
    draw_sel_cell ();
}

void draw_current_menu () {
    drawTitle ("POWER");

    Graphics_drawImage(&g_sContext, &SX, offset_x_SX, offset_y_SX);
    Graphics_drawImage(&g_sContext, &DOWN, offset_x_DOWN, offset_y_DOWN);
    Graphics_drawImage(&g_sContext, &DX, offset_x_DX, offset_y_DX);

    drawStringMid (" Pratical  Theorical", offset_y_subtitle);

    // graphics for the theorical      ----------------------------------


    // graphics for registered current ----------------------------------


    Graphics_drawLineV(&g_sContext, offset_x_mid, start_y_vline_a, end_y_vline);

    // graphics for timeframes table   ----------------------------------
    table_draw ();
}

void draw_mqtt_menu () {
    drawTitle ("MQTT");

    Graphics_drawImage(&g_sContext, &UP, offset_x_UP, offset_y_UP);
    Graphics_drawImage(&g_sContext, &DOWN, offset_x_DOWN, offset_y_DOWN);
    Graphics_drawImage(&g_sContext, &DX, offset_x_DX, offset_y_DX);

    // graphics for status
    drawStringMid (status_mqtt[current_state_mqtt], offset_y_subtitle);

    // graphics for min and max
    drawStringMid ("MIN     MAX", offset_y_mqtt_2);

    char string[20];

    // DATE
    switch (curr_tf_mqtt) {
        case DD:
            sprintf(string, "%02d hh    %02d hh", mqttBuffer[0][0], mqttBuffer[1][0]);
            drawStringMid (string, offset_y_mqtt_3);
            sprintf(string, "%02d/%02d/%02d %02d/%02d/%02d", mqttBuffer[0][1], mqttBuffer[0][2], mqttBuffer[0][3] %100, mqttBuffer[1][1], mqttBuffer[1][2], mqttBuffer[1][3] %100);
            drawStringMid (string, offset_y_mqtt_4);
            break;
        case MM:
            drawStringMid (no_date, offset_y_mqtt_3);
            sprintf(string, "%02d/%02d/%02d %02d/%02d/%02d", mqttBuffer[0][1], mqttBuffer[0][2], mqttBuffer[0][3] %100, mqttBuffer[1][1], mqttBuffer[1][2], mqttBuffer[1][3] %100);
            drawStringMid (string, offset_y_mqtt_4);
            break;
        case YY:
        case AT:
            drawStringMid (no_date, offset_y_mqtt_3);
            sprintf(string, "%03s/%4d %03s/%4d", MONTH[mqttBuffer[0][2]], mqttBuffer[0][3], MONTH[mqttBuffer[1][2]], mqttBuffer[1][3]);
            drawStringMid (string, offset_y_mqtt_4);
            break;
    }

    // DATA
    sprintf(string, "%4d W   %4d W", mqttBuffer[0][4], mqttBuffer[1][4]);
    drawStringMid (string, offset_y_mqtt_5);

    Graphics_drawLineV(&g_sContext, offset_x_mid, start_y_vline_b, end_y_vline);

    // graphics for timeframes mqtt table -------------------------------------
    table_draw ();
}

void draw_sensors_menu () {
    drawTitle ("SENSORS");

    Graphics_drawImage(&g_sContext, &UP, offset_x_UP, offset_y_UP);
    Graphics_drawImage(&g_sContext, &SX, offset_x_SX, offset_y_SX);
    Graphics_drawImage(&g_sContext, &DX, offset_x_DX, offset_y_DX);

    char string[20];

    sprintf(string, " Temp: %3d,%02d °C ", sensorsBuffer[0], sensorsBuffer[1]);
    drawStringMid (string, offset_y_sensors_temperature);

    // overload of 4 spaces
    if (sensorsBuffer[4] <= 9999)
        sprintf(string, "Light: %4d lux", sensorsBuffer[4]);
    else
        sprintf(string, "Light: %3dk lux", sensorsBuffer[4] /1000);
    drawStringMid (string, offset_y_sensors_lux);

    sprintf(string, " Humid: %2d,%02d %%  ", sensorsBuffer[2], sensorsBuffer[3]);
    drawStringMid (string, offset_y_sensors_humidity);

    // graphics for timeframes table -------------------------------------
    table_draw ();
}

void draw_alarm_menu () {
    drawTitle ("ALARM");

    Graphics_drawImage(&g_sContext, &UP, offset_x_UP, offset_y_UP);
    Graphics_drawImage(&g_sContext, &SX, offset_x_SX, offset_y_SX);
    Graphics_drawImage(&g_sContext, &DOWN, offset_x_DOWN, offset_y_DOWN);

    drawStringCentered_Alarm ("LED", offset_y_alarm_text);
    Graphics_drawRectangle(&g_sContext, &(Graphics_Rectangle){
                                offset_x_alarm_label,
                                offset_y_alarm_label,
                                offset_x_alarm_label + offset_interaction,
                                offset_y_alarm_label + offset_interaction,
    });
    if (alarm_led)
        Graphics_drawImage(&g_sContext, &BLACK_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + 1);
    else
        Graphics_drawImage(&g_sContext, &WHITE_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + 1);

    drawStringCentered_Alarm ("MQTT", offset_y_alarm_text + inter_text);
    Graphics_drawRectangle(&g_sContext, &(Graphics_Rectangle){
                                offset_x_alarm_label,
                                offset_y_alarm_label + offset_interaction,
                                offset_x_alarm_label + offset_interaction,
                                offset_y_alarm_label + 2 * offset_interaction,
    });
    if (alarm_mqtt)
        Graphics_drawImage(&g_sContext, &BLACK_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + offset_interaction + 1);
    else
        Graphics_drawImage(&g_sContext, &WHITE_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + offset_interaction + 1);

    drawStringCentered_Alarm ("ACTIVE", offset_y_alarm_text + 2 * inter_text + 10);
    Graphics_drawRectangle(&g_sContext, &(Graphics_Rectangle){
                                offset_x_alarm_label,
                                offset_y_alarm_label + 2 * offset_interaction + 10,
                                offset_x_alarm_label + offset_interaction,
                                offset_y_alarm_label + 3 * offset_interaction + 10,
    });
    if (alarm_active)
        Graphics_drawImage(&g_sContext, &BLACK_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + 2 * offset_interaction + 11);
    else
        Graphics_drawImage(&g_sContext, &WHITE_CELL, offset_x_alarm_label + 1, offset_y_alarm_label + 2 * offset_interaction + 11);
}

void newBackGround () {
    Graphics_setBackgroundColor(&g_sContext, BACKGROUNG_COLOR);
}

void alarm_screen () {
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_DARK_RED);
    Graphics_clearDisplay(&g_sContext);

    char string[61] = "ALARM: Press a button to stop";

    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)string,
                                AUTO_STRING_LENGTH,
                                64, 64,
                                OPAQUE_TEXT);
}
