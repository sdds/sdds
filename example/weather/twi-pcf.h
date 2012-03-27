#ifndef TWI_PCF_H_INCLUDED
#define TWI_PCF_H_INCLUDED

#include <stdint.h>

/*
 * two wire interface module for phillips pcf8591p
 * -----------------------------------------------
 *
 * a module to allow easy access to a pcf8591p
 * call twi_init_pcf first
 */
int twi_init_pcf(uint8_t channel);
int twi_read_pcf(uint8_t *value);

#endif
