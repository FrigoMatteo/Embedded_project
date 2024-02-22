/*
 * status.h
 *
 * state of the program variables and declaration
 *
 */


#ifndef STATUS_H
#define STATUS_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef enum {
    UP_STATE,
    DOWN_STATE,
    LEFT_STATE,
    RIGHT_STATE,
}State_t;

extern State_t current_state;

typedef enum {
    M_15, // 15 MIN
    M_30, // 30 MIN
    H_1,  // 1  HOUR
    D_1,  // I DAY
}Time_agg_t;

typedef enum {
    DD, // DAY
    MM, // MONTH
    YY, // YEAR
    AT, // ALL TIME
}Time_maxmin_t;

extern Time_agg_t curr_tf;
extern Time_agg_t sel_tf;
extern Time_maxmin_t curr_tf_mqtt;
extern Time_maxmin_t sel_tf_mqtt;

typedef enum {
    CONNECTED,
    OFFLINE,
} State_mqtt_t;

typedef enum {
    WHITE_BLACK,
    GREEN_BLACK,
    BLUE_BLACK,
    PURPLE_BLACK,
}Color_t;

extern Color_t color;

// THEORIC VALUES
extern const uint16_t A;
extern const uint16_t B;
extern const uint16_t C;

uint16_t minCurrent (uint16_t* buffer);


#endif  // STATUS_H
