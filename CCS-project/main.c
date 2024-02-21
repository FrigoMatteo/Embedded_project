#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "msp.h"

#include <stdio.h>
#include <string.h>

#include <Hardware/LcdDriver/Crystalfontz128x128_ST7735.h>
#include <Hardware/Graphics/menu_graphics.h>

#include <Hardware/HumTempDriver/HAL_DHT20.h>
#include <Hardware/hardware_init.h>
#include <Hardware/LuxDriver/HAL_OPT3001.h>
#include <Hardware/status.h>

#define change_screen (*fsm[current_state].state_function)()


#define TESTING_JOYSTICK 0
#define TESTING_UART 0
#define debouncer_delay 4000000
#define MAX_SIZE_READ 64
#define SAMPLING_BUFFER_SIZE 6

const uint16_t Time_agg_int[4] = {15, 30, 60, 1440};

// -------------------------    ADC results buffer
uint16_t joystickBuffer[2]; // [J1, J2]
uint16_t mqttBuffer[2][5];  // [min, max][h, day, month, year, curr]
uint16_t sensorsBuffer[SAMPLING_BUFFER_SIZE];  // [TEMPERATURE x2, HUMIDITY x2, LUX, CURRENT]
uint16_t samplingBuffer[SAMPLING_BUFFER_SIZE];
uint32_t sumSamplingBuffer[SAMPLING_BUFFER_SIZE];

// -------------------------    global variables for timeframes
Time_agg_t curr_tf = M_15;
Time_agg_t sel_tf = M_15;

Time_maxmin_t curr_tf_mqtt = DD;
Time_maxmin_t sel_tf_mqtt = DD;

// -------------------------    state
State_t current_state = UP_STATE;
State_mqtt_t current_state_mqtt = OFFLINE;
Color_t color = WHITE_BLACK;
uint8_t timer_counter = 0;
uint8_t hour_counter=0;

// -------------------------    alarm state
bool alarm_led = false;
bool alarm_mqtt = false;
bool alarm_active = false;
bool alarm_running = false;

// ------------------------- UART communication
uint8_t RXData = 0;
char esp_data[MAX_SIZE_READ] = "";
uint8_t count=0;
uint8_t ack=0;

// -------------------------    fms architecture
void UP_funz(){
    draw_current_menu ();
}

void LEFT_funz(){
    draw_mqtt_menu ();
}

void DOWN_funz(){
    draw_sensors_menu ();
}

void RIGHT_funz(){
    draw_alarm_menu ();
}

// state machine
typedef struct{
    State_t state;
    void (*state_function)();
} StateMachine_t;

StateMachine_t fsm[] = {
      {UP_STATE, UP_funz},
      {DOWN_STATE, DOWN_funz},
      {LEFT_STATE, LEFT_funz},
      {RIGHT_STATE, RIGHT_funz}
};

// main
void main () {

    
#if TESTING_JOYSTICK==1
    //Testing function for our JOYSTICK function:
    // It will constantly select the following current_state of our machine state.
    while (1){
        ISR_testing_JoyStick();
        __delay_cycles(10000000);
        switch (current_state){
            case UP_STATE: current_state=DOWN_STATE; break;
            case DOWN_STATE:current_state=RIGHT_STATE;break;
            case RIGHT_STATE: current_state=LEFT_STATE;break;
            case LEFT_STATE:current_state=UP_STATE;break;
        }
    }
#endif
#if TESTING_UART==1
     //Testing function for our JOYSTICK function:
        // It will constantly select the following current_state of our machine state.
        esp_data[0]='1';
        esp_data[1]='5';
        esp_data[2]='\0';
        while (1){
            ISR_testing_UART();
            __delay_cycles(10000000);
        }
#else

     //Normal usage of the MSP432
     hwInit();
     esp_data[0]='\0';
     change_screen;
     while (1) {
         PCM_gotoLPM0InterruptSafe();
     }

#endif

}


//Here I do the testing of the read UART.
//We basically, create a random set of char as it comes from ESP32.
//Then we try to write back to our ESP32.

void ISR_testing_UART(){
    printf("We send back an ACK%c%c",esp_data[0],esp_data[1]);
    //I modify the rate of check for values based on the web data.
    if (strcmp(esp_data,"15")==0){
        //We set the following esp_data:
        esp_data[0]='3';
        esp_data[1]='0';
        esp_data[2]='\0';

        //
        ack = 1;
        curr_tf = M_15;
        sel_tf = M_15;
    } else if (strcmp(esp_data,"30") == 0) {

        esp_data[0]='6';
        esp_data[1]='0';
        esp_data[2]='\0';

        ack = 1;
        curr_tf = M_30;
        sel_tf = M_30;
    } else if (strcmp(esp_data, "60") == 0) {

        esp_data[0]='1';
        esp_data[1]='D';
        esp_data[2]='\0';


        ack = 1;
        curr_tf = H_1;
        sel_tf = H_1;
    } else if (strcmp(esp_data, "1D") == 0) {


        esp_data[0]='S';
        esp_data[1]='W';
        esp_data[2]='\0';

        ack = 1;
        curr_tf = D_1;
        sel_tf = D_1;
    } else if (strcmp(esp_data, "SW") == 0) {       // A web app is started

        esp_data[0]='A';
        esp_data[1]='S';
        esp_data[2]='\0';


        switch (curr_tf) {
        case  M_15:
            printf("Write back 15#");
            break;
        case  M_30:
            printf("Write back 30#");
            break;
        case  H_1:
            printf("Write back 60#");
            break;
        case  D_1:
            printf("Write back 1D#");
            break;
        }
    } else if (strcmp(esp_data, "AS") == 0) {       // Alarm Stop from a web app

        esp_data[0]='m';
        esp_data[1]='n';
        esp_data[2]='\0';

        stop_alarm ();
        newBackGround ();
        new_state ();
    } else if (strncmp(esp_data, "mn", 2) == 0) {
        esp_data[0]='1';
        esp_data[1]='5';
        esp_data[2]='\0';
        printf("Received data for LEFT_STATE(hour, day, month, year etc.)\n");
    }
    change_screen;
}


//Here we are going to test the joystick program,
// by giving some random input to it.
void ISR_testing_JoyStick(){
        if (!alarm_running) {
            switch (current_state) {
                case UP_STATE:
                    // sono uguali
                case DOWN_STATE:
                    // CONFERMO timeframeA
                    switch (sel_tf) {
                        case M_15:
                            curr_tf = M_15;
                            printf("Send 15 to ESP32");     
                            break;
                        case M_30:
                            curr_tf = M_30;
                            printf("Send 30 to ESP32");
                            break;
                        case  H_1:
                            curr_tf = H_1;
                            printf("Send 60 to ESP32");
                            break;
                        case  D_1:
                            curr_tf = D_1;
                            printf("Send 1D to ESP32");
                            break;
                        }
                        break;
                    case RIGHT_STATE:
                        // toggle alarm
                        alarm_active = ! alarm_active;
                        led_alarm();
                        break;
                    case LEFT_STATE:
                        // CONFERMO mqtt timeFrame
                        switch (sel_tf_mqtt) {
                            case DD:
                                curr_tf_mqtt = DD;
                                printf("Send DD to ESP32");
                                break;
                            case MM:
                                curr_tf_mqtt = MM;
                                printf("Send MM to ESP32");
                                break;
                            case YY:
                                curr_tf_mqtt = YY;
                                printf("Send YY to ESP32");
                                break;
                            case AT:
                                curr_tf_mqtt = AT;
                                printf("Send AT to ESP32");
                                break;
                        }
                }
            } else {
                stop_alarm();
            }
            change_screen;
}



void new_state () {
    Graphics_clearDisplay(&g_sContext);
    change_screen;
}

void led_alarm () {
    if (alarm_active && alarm_led) {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);    // turn off red led
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);   // turn on green led
    } else {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);    // turn off red led
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);    // turn off green led
    }
}

void start_alarm () {
    alarm_running = true;

    // if led active
    if (alarm_led) {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);    // turn off green led
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);    // turn on red led
    }

    // if mqtt active
    if (alarm_mqtt) {
        UART_transmitData(EUSCI_A2_BASE, '!');
        UART_transmitData(EUSCI_A2_BASE, '!');
        UART_transmitData(EUSCI_A2_BASE, '#');
    }

    //change LCD
    alarm_screen ();
}


void stop_alarm () {
    alarm_running = false;
    alarm_active = false;

    // led
    led_alarm ();

    // mqtt
    if (alarm_mqtt) {
        UART_transmitData(EUSCI_A2_BASE, 'A');
        UART_transmitData(EUSCI_A2_BASE, 'S');
        UART_transmitData(EUSCI_A2_BASE, '#');
    }

    // LCD
    newBackGround ();
}


void EUSCIA2_IRQHandler(void) {
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        RXData=UART_receiveData(EUSCI_A2_BASE);

        if (RXData != '#') {
            esp_data[count] = RXData;
            count++;
        } else{
            esp_data[count] = '\0';
            count=0;
            printf("esp_data:%s\n",esp_data);

            //I modify the rate of check for values based on the web data.
            if (strcmp(esp_data,"15")==0){
                ack = 1;
                curr_tf = M_15;
                sel_tf = M_15;
            } else if (strcmp(esp_data,"30") == 0) {
                ack = 1;
                curr_tf = M_30;
                sel_tf = M_30;
            } else if (strcmp(esp_data, "60") == 0) {
                ack = 1;
                curr_tf = H_1;
                sel_tf = H_1;
            } else if (strcmp(esp_data, "1D") == 0) {
                ack = 1;
                curr_tf = D_1;
                sel_tf = D_1;
            } else if (strcmp(esp_data, "SW") == 0) {       // A web app is started
                printf("web_app activated\n");
                switch (curr_tf) {
                    case  M_15:
                        UART_transmitData(EUSCI_A2_BASE, '1');
                        UART_transmitData(EUSCI_A2_BASE, '5');
                        UART_transmitData(EUSCI_A2_BASE, '#');
                        break;
                    case  M_30:
                        UART_transmitData(EUSCI_A2_BASE, '3');
                        UART_transmitData(EUSCI_A2_BASE, '0');
                        UART_transmitData(EUSCI_A2_BASE, '#');
                        break;
                    case  H_1:
                        UART_transmitData(EUSCI_A2_BASE, '6');
                        UART_transmitData(EUSCI_A2_BASE, '0');
                        UART_transmitData(EUSCI_A2_BASE, '#');
                        break;
                    case  D_1:
                        UART_transmitData(EUSCI_A2_BASE, '1');
                        UART_transmitData(EUSCI_A2_BASE, 'D');
                        UART_transmitData(EUSCI_A2_BASE, '#');
                        break;
                }
            } else if (strcmp(esp_data, "AS") == 0) {       // Alarm Stop from a web app
                stop_alarm ();
                newBackGround ();
                new_state ();
            } else if (strncmp(esp_data, "mn", 2) == 0) {
                printf("data0\n");
                sscanf(esp_data, "mn%2hu%2hu%2hu%4hu%4huMX%2hu%2hu%2hu%4hu%4hu",
                       &mqttBuffer[0][0], &mqttBuffer[0][1], &mqttBuffer[0][2],
                       &mqttBuffer[0][3], &mqttBuffer[0][4], &mqttBuffer[1][0],
                       &mqttBuffer[1][1], &mqttBuffer[1][2], &mqttBuffer[1][3],
                       &mqttBuffer[1][4]);
                printf("data1\n");
            }

            change_screen;

            //We send back an ACK. It's based if we received the data correctly.
            if (ack==1){
                UART_transmitData(EUSCI_A2_BASE,'A');
                UART_transmitData(EUSCI_A2_BASE,'C');
                UART_transmitData(EUSCI_A2_BASE,esp_data[0]);
                UART_transmitData(EUSCI_A2_BASE,esp_data[1]);
                ack=0;
            }
            esp_data[0] = '\0';
        }
        Interrupt_disableSleepOnIsrExit();
    }
}



// timer interrupt
void TA0_N_IRQHandler(void){


    Timer_A_clearInterruptFlag(TIMER_A0_BASE);

    timer_counter++;
    printf("timer: %d\n", timer_counter);
    hour_counter++;

    DHT20_getTempHumid (samplingBuffer); //temp and humid [0,1,2,3]
    samplingBuffer[4] = OPT3001_getLux(); // light
    // samplingBuffer[5] = ; // current

    int i;
    for (i = 0; i < SAMPLING_BUFFER_SIZE; ++i)
        sumSamplingBuffer[i] += samplingBuffer[i];

    if (timer_counter >= Time_agg_int[curr_tf]) {

        for (i = 0; i < SAMPLING_BUFFER_SIZE; ++i){
            sensorsBuffer[i] = sumSamplingBuffer[i];
            sumSamplingBuffer[i] = 0;
        }

        int temp = (sensorsBuffer[0] *100 + sensorsBuffer[1]) / timer_counter;
        sensorsBuffer[0] = temp /100;
        sensorsBuffer[1] = temp %100;

        temp = (sensorsBuffer[2] *100 + sensorsBuffer[3]) / timer_counter;
        sensorsBuffer[2] = temp /100;
        sensorsBuffer[3] = temp %100;

        sensorsBuffer[4] /= timer_counter;

        timer_counter = 0;
        change_screen;
        if (alarm_active) {
            start_alarm ();
        }
    }

}

/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM1. This signals the end of conversion and the results array is
 * grabbed and placed in joystickBuffer */
void ADC14_IRQHandler () {
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    /* ADC_MEM1 conversion completed */
    if(status & ADC_INT1 && !alarm_running) {
        /* Store ADC14 conversion results */
        joystickBuffer[0] = ADC14_getResult(ADC_MEM0);
        joystickBuffer[1] = ADC14_getResult(ADC_MEM1);

        //se joystickBuffer[0] > 6000 && joystickBuffer[0] < 11000
        if (joystickBuffer[0] > 6000 && joystickBuffer[0] < 11000){
            if (joystickBuffer[1] > 13000 && current_state != UP_STATE) {
                current_state = UP_STATE;
                new_state ();
            }
            else if (joystickBuffer[1] < 3000 && current_state != DOWN_STATE) {
                current_state = DOWN_STATE;
                new_state ();
            }
        } else if (joystickBuffer[1] > 6000 && joystickBuffer[1] < 11000){
            if (joystickBuffer[0] > 13000 && current_state != RIGHT_STATE) {
                current_state = RIGHT_STATE;
                new_state ();
            }
            else if (joystickBuffer[0] < 3000 && current_state != LEFT_STATE) {
                current_state = LEFT_STATE;
                new_state ();
            }
        }
    }
}

// B
void PORT3_IRQHandler () {
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if (status & GPIO_PIN5) {
        if (!alarm_running) {
            switch (current_state) {
                case UP_STATE:
                    // sono uguali
                case DOWN_STATE:
                    // cambio timeframeA
                    switch (sel_tf) {
                        case M_15:  sel_tf = (curr_tf == D_1) ? H_1 : D_1; break;
                        case M_30:  sel_tf = (curr_tf == M_15) ? D_1 : M_15; break;
                        case  H_1:  sel_tf = (curr_tf == M_30) ? M_15 : M_30; break;
                        case  D_1:  sel_tf = (curr_tf == H_1) ? M_30 : H_1; break;
                    } break;
                case RIGHT_STATE:
                    // attivo MQTT alarm
                    alarm_mqtt = ! alarm_mqtt;
                    break;
                case LEFT_STATE:
                    // cambio timeframeB
                    switch (sel_tf_mqtt) {
                        case DD: sel_tf_mqtt = (curr_tf_mqtt == AT) ? YY : AT; break;
                        case MM: sel_tf_mqtt = (curr_tf_mqtt == DD) ? AT : DD; break;
                        case YY: sel_tf_mqtt = (curr_tf_mqtt == MM) ? DD : MM; break;
                        case AT: sel_tf_mqtt = (curr_tf_mqtt == YY) ? MM : YY; break;
                    }
            }
        } else {
            stop_alarm();
        }
        change_screen;

        // debouncer (expensive)
        __delay_cycles(debouncer_delay);
        GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    }
}

// J
void PORT4_IRQHandler () {
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);

    if (status & GPIO_PIN1) {
        if (!alarm_running) {
            switch (current_state) {
                case UP_STATE:
                    // sono uguali
                case DOWN_STATE:
                    // CONFERMO timeframeA
                    switch (sel_tf) {
                        case M_15:
                            curr_tf = M_15;
                            UART_transmitData(EUSCI_A2_BASE, '1');
                            UART_transmitData(EUSCI_A2_BASE, '5');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case M_30:
                            curr_tf = M_30;
                            UART_transmitData(EUSCI_A2_BASE, '3');
                            UART_transmitData(EUSCI_A2_BASE, '0');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case  H_1:
                            curr_tf = H_1;
                            UART_transmitData(EUSCI_A2_BASE, '6');
                            UART_transmitData(EUSCI_A2_BASE, '0');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case  D_1:
                            curr_tf = D_1;
                            UART_transmitData(EUSCI_A2_BASE, '1');
                            UART_transmitData(EUSCI_A2_BASE, 'D');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                    }
                    break;
                case RIGHT_STATE:
                    // toggle alarm
                    alarm_active = ! alarm_active;
                    led_alarm();
                    break;
                case LEFT_STATE:
                    // CONFERMO mqtt timeFrame
                    switch (sel_tf_mqtt) {
                        case DD:
                            curr_tf_mqtt = DD;
                            UART_transmitData(EUSCI_A2_BASE, 'D');
                            UART_transmitData(EUSCI_A2_BASE, 'D');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case MM:
                            curr_tf_mqtt = MM;
                            UART_transmitData(EUSCI_A2_BASE, 'M');
                            UART_transmitData(EUSCI_A2_BASE, 'M');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case YY:
                            curr_tf_mqtt = YY;
                            UART_transmitData(EUSCI_A2_BASE, 'Y');
                            UART_transmitData(EUSCI_A2_BASE, 'Y');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                        case AT:
                            curr_tf_mqtt = AT;
                            UART_transmitData(EUSCI_A2_BASE, 'A');
                            UART_transmitData(EUSCI_A2_BASE, 'T');
                            UART_transmitData(EUSCI_A2_BASE, '#');
                            break;
                    }
            }
        } else {
            stop_alarm();
        }
        change_screen;

        // debouncer (expensive)
        __delay_cycles(debouncer_delay);
        GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
    }
}


// A
void PORT5_IRQHandler () {
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    if (status & GPIO_PIN1) {
        if (!alarm_running) {
            switch (current_state) {
                case UP_STATE:
                    // sono uguali
                case DOWN_STATE:
                    // cambio timeframeA
                    switch (sel_tf) {
                        case M_15:  sel_tf = (curr_tf == M_30) ? H_1 : M_30; break;
                        case M_30:  sel_tf = (curr_tf == H_1) ? D_1 : H_1; break;
                        case  H_1:  sel_tf = (curr_tf == D_1) ? M_15 : D_1; break;
                        case  D_1:  sel_tf = (curr_tf == M_15) ? M_30 : M_15; break;
                    } break;
                case RIGHT_STATE:
                    // attivo led alarm
                    alarm_led = ! alarm_led;
                    led_alarm();
                    break;
                case LEFT_STATE:
                    // cambio timeframeB
                    switch (sel_tf_mqtt) {
                        case DD: sel_tf_mqtt = (curr_tf_mqtt == MM) ? YY : MM; break;
                        case MM: sel_tf_mqtt = (curr_tf_mqtt == YY) ? AT : YY; break;
                        case YY: sel_tf_mqtt = (curr_tf_mqtt == AT) ? DD : AT; break;
                        case AT: sel_tf_mqtt = (curr_tf_mqtt == DD) ? MM : DD; break;
                    }
            }
        } else {
            stop_alarm();
        }
        change_screen;

        // debouncer (expensive)
        __delay_cycles(debouncer_delay);
        GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    }
}

// C
void PORT1_IRQHandler () {
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN4 && !alarm_running) {
        // change color
        color = (color +1) %4;

        newBackGround ();
        new_state ();

        // debouncer (expensive)
        __delay_cycles(debouncer_delay);
        GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    }
}

