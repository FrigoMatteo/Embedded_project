/*
 * HAL_INA219.h
 *
 * hardware abstraction layer for ina219
 *
 */

#ifndef __HAL_INA219_H_
#define __HAL_INA219_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// constants
#define INA219_ADDRESS 0x40

#define CONFIGURATION_REGISTER 0x00
#define CURRENT_REGISTER 0x01
#define CALIBRATION_REGISTER 0x05

// fn
uint16_t ina219_getCurrent ();
void ina219_init ();

#endif /* __HAL_INA219_H_ */
