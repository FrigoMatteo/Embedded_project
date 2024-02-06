#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "HAL_I2C.h"
#include "HAL_OPT3001.h"
#include "HAL_TMP006.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>



uint32_t cal30;
uint32_t cal85;
float lux;
float temp;
float calDifference;
float tempC;


void _temperatureSensorInit()
{
    /* Temperature Sensor initialization */
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();
    /* Initialize TMP006 temperature sensor */
    TMP006_init();
    __delay_cycles(100000);

}

void _lightSensorInit()
{
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();

    /* Initialize OPT3001 digital ambient light sensor */
    OPT3001_init();

    __delay_cycles(100000);

}


void _secondSensor(){
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();

    /* Initialize OPT3001 digital ambient light sensor */
    OPT3001_init();

    __delay_cycles(100000);
}


int main(void)
{
    WDT_A_holdTimer();
        _lightSensorInit();
        _temperatureSensorInit();

        printf("ccc\n");
        _init_DHT();


        REF_A_setReferenceVoltage(REF_A_VREF2_5V);

        cal30 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,
                                                      SYSCTL_30_DEGREES_C);
        cal85 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,
                                                      SYSCTL_85_DEGREES_C);
        calDifference = cal85 - cal30;

        float tempSecond;
        while (1){
            read_DHT_sensor();


            lux=OPT3001_getLux();

            temp=((TMP006_getTemp()-32)*5)/9;

            tempSecond=GPIO_getInputPinValue(GPIO_PORT_P3,GPIO_PIN6);



            //printf("Value of lux:%f\n",lux);
            //printf("Value of temp:%f\n",temp);
            printf("temp:%f\n",temp);

            __delay_cycles(3000000);

            //__delay_cycles(10000000);
        }

}
