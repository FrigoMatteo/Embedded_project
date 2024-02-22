/*
 * status.c
 *
 * current Threshold calculation
 *
 */

#include <Hardware/status.h>

uint16_t minCurrent (uint16_t* buffer) {
    return A * buffer[0] + B * buffer[2] + C * buffer[4];
}
