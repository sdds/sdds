#ifndef EXPERIMENTS_H_INCLUDED
#define EXPERIMENTS_H_INCLUDED

#include <stdint.h>

#define TWI_TI0_ADDRESS 0xc2
#define TWI_TI1_ADDRESS 0xc4
#define TWI_TI2_ADDRESS 0xc8

/* initialize the light sensor
 *
 * @return 0 on success, 1 on failure
 */
int init_light(void);

/* read the current value from the light sensor
 * higher values represent darker environments
 *
 * @param value light intensity [0,256)
 *
 * @return 0 on success, 1 on failure
 */
int read_light(uint8_t *value);

int swrst_ti(void);
int setup_ti_one(uint8_t address);
int setup_ti_all(void);
int control_ti_led(uint8_t address, uint8_t led_index, uint8_t byte);

#endif
