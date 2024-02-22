/*
 * HAL_INA219.c
 *
 * hardware abstraction layer for ina219
 *
 */

#include <Hardware/I2cDriver/HAL_I2C.h>
#include "HAL_INA219.h"

#define I2C_MODULE EUSCI_B1_BASE
#define sizeof_output 2

uint16_t ina219_getCurrent () {
    uint8_t buf[sizeof_output];

    I2C_setslave  (INA219_ADDRESS);
    I2C_setMode (I2C_MODULE, EUSCI_B_I2C_RECEIVE_MODE);

    // START
    I2C_masterReceiveStart (I2C_MODULE);
    while(!(I2C_getInterruptStatus(I2C_MODULE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));
    buf[0] = I2C_masterReceiveMultiByteNext(I2C_MODULE);
    buf[1] = I2C_masterReceiveMultiByteFinish(I2C_MODULE);
    // STOP
    while (I2C_isBusBusy(I2C_MODULE));

    uint16_t current_raw = ((uint32_t)buf[0] << 8 |
                          (uint32_t)buf[1] << 0);

    // mA
    return current_raw;
}

void ina219_init () {
    // set I2C
    I2C_setslave  (INA219_ADDRESS);
    I2C_setMode (I2C_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);

    // ------------------------------------------------ CONFIGURATION REGISTER
    I2C_masterSendMultiByteStart (I2C_MODULE, CONFIGURATION_REGISTER);
    I2C_masterSendMultiByteNext (I2C_MODULE, 0x39);
    I2C_masterSendMultiByteFinish (I2C_MODULE, 0x9f);
    // finish send
    while (I2C_isBusBusy(I2C_MODULE));
    // 4 us to make the change
    __delay_cycles (192000);

    // ------------------------------------------------ CALIBRATION REGISTER
    I2C_masterSendMultiByteStart (I2C_MODULE, CALIBRATION_REGISTER);
    I2C_masterSendMultiByteNext (I2C_MODULE, 0x10);
    I2C_masterSendMultiByteFinish (I2C_MODULE, 0x00);
    // finish send
    while (I2C_isBusBusy(I2C_MODULE));
    // 4 us to make the change
    __delay_cycles (192000);

    // -------------------------------------------------- SET REGISTER POINTER
    I2C_masterSendSingleByte (I2C_MODULE, CURRENT_REGISTER);
    // finish send
    while (I2C_isBusBusy(I2C_MODULE));
    // 4 us to make the change
    __delay_cycles (192000);
}
