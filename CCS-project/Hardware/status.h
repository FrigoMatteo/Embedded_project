// status.h
#ifndef STATUS_H
#define STATUS_H


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
    ERROR,
} State_mqtt_t;

typedef enum {
    WHITE_BLACK,
    GREEN_BLACK,
    BLUE_BLACK,
    PURPLE_BLACK,
}Color_t;

extern Color_t color;


#endif  // STATUS_H
