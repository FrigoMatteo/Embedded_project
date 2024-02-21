

#ifndef __HAL_DHT20_H_
#define __HAL_DHT20_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// constants
#define DHT20_ADDRESS 0x38
#define sizeof_output 7
#define I2C_MODULE EUSCI_B1_BASE

// fn
void DHT20_getTempHumid (uint16_t* buffer);
void dht20_init ();

#endif /* __HAL_DHT20_H_ */
