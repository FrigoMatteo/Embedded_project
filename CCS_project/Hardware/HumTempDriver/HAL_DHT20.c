/*
 * HAL_DHT20.c
 *
 * hardware abstraction layer for dht20
 *
 */

#include <Hardware/I2cDriver/HAL_I2C.h>
#include "HAL_DHT20.h"

#define I2C_MODULE EUSCI_B1_BASE
#define sizeof_output 7

// check control bit
static uint8_t a_dht20_calc_crc(uint8_t *data, uint8_t len) {
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;

    for (byte = 0; byte < len; byte++) {        /* len times */
        crc ^= data[byte];                      /* xor byte */
        for (i = 8; i > 0; --i) {               /* one byte */
            if ((crc & 0x80) != 0) {            /* if high*/
                crc = (crc << 1) ^ 0x31;        /* xor 0x31 */
            } else {
                crc = crc << 1;                 /* skip */
            }
        }
    }
    return crc;                                 /* return crc */
}

void DHT20_getTempHumid (uint16_t* buffer) {
    uint8_t buf[sizeof_output];

    buf[0] = 0xAC;                                                    /* set the addr */
    buf[1] = 0x33;                                                    /* set 0x33 */
    buf[2] = 0x00;                                                    /* set 0x00 */

    // init adress for slave
    I2C_setslave  (DHT20_ADDRESS);

    /* Set master to transmit mode PL */
    I2C_setMode (I2C_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Initiate start and send text and stop */
    I2C_masterSendMultiByteStart (I2C_MODULE, buf[0]);
    I2C_masterSendMultiByteNext (I2C_MODULE, buf[1]);
    I2C_masterSendMultiByteFinish (I2C_MODULE, buf[2]);

    while (I2C_isBusBusy(I2C_MODULE));

    // delay for mesuring
    __delay_cycles (4080000);

    /* Read from I2C RX register */
    I2C_setMode (I2C_MODULE, EUSCI_B_I2C_RECEIVE_MODE);
    I2C_masterReceiveStart (I2C_MODULE);

    int i;
    for (i = 0; i < sizeof_output -1; ++i) {
        while(!(I2C_getInterruptStatus(I2C_MODULE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));
        buf[i] =  I2C_masterReceiveMultiByteNext(I2C_MODULE);

    }
    buf[6] = I2C_masterReceiveMultiByteFinish(I2C_MODULE);

    // fail
    if (a_dht20_calc_crc(buf, sizeof_output -1) != buf[6]) return;

    /* set the humidity */
    uint32_t humidity_raw = (((uint32_t)buf[1]) << 16) |
                            (((uint32_t)buf[2]) << 8) |
                            (((uint32_t)buf[3]) << 0);
    humidity_raw = humidity_raw >> 4;                         /* right shift 4 */

    buffer[2] = humidity_raw *25 >>18;
    buffer[3] = (humidity_raw *25 <<14 >>12) *25 >>18;

    /* set the temperature */
    uint32_t temperature_raw = (((uint32_t)buf[3]) << 16) |
                               (((uint32_t)buf[4]) << 8) |
                               (((uint32_t)buf[5]) << 0);
    temperature_raw = temperature_raw & 0xFFFFF;              /* cut the temperature part */

    buffer[0] = (temperature_raw *25 >>17) -50;
    buffer[1] = (temperature_raw *25 <<15 >>12) *25 >>18;
}

void dht20_init () {
    __delay_cycles (40800000);

    I2C_setslave  (DHT20_ADDRESS);
    I2C_setMode (I2C_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);

    I2C_masterSendSingleByte (I2C_MODULE, 0x71);
    while (I2C_isBusBusy(I2C_MODULE));

    uint8_t status  =   I2C_masterReceiveSingleByte(I2C_MODULE);
    while (I2C_isBusBusy(I2C_MODULE));
}
