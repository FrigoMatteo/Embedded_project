
#ifndef __HAL_INA219_H_
#define __HAL_INA219_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// constants
#define INA219_ADDRESS 0x40
#define I2C_MODULE EUSCI_B1_BASE

// fn
void ina219_getPower ();
void ina219_init ();

#endif /* __HAL_INA219_H_ */
