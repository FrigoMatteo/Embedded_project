/*
 * HAL_DHT20.h
 *
 * hardware abstraction layer for dht20
 *
 */

#ifndef __HAL_DHT20_H_
#define __HAL_DHT20_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// constants
#define DHT20_ADDRESS 0x38

// fn
void DHT20_getTempHumid (uint16_t* buffer);
void dht20_init ();

#endif /* __HAL_DHT20_H_ */
