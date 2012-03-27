#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include "twi-type.h"

#include <stdint.h>

/*
 * two wire interface module
 * -------------------------
 *
 * after twi_init you can build twi_data datastructures
 * and pass them to twi_communicate
 * to configure the i/o pins and frequency check 'twi-config.h'
 */

struct twi_data
{
	enum twi_type type;
	uint8_t byte;
};

typedef struct twi_data twi_data_t;

/*
 * initialize the twi
 */
void twi_init(void);

/*
 * communicate using the twi module
 *
 * @param data an array of twi_data command/byte pairs
 * @param amount the amount of passed array members
 * @param failed_command an output parameter, which will tell which command failed, if any
 *
 * @return 0, 1, -1 or 2
 *         0 on success
 *         1 if arbitration was lost
 *         -1 on protocol errors
 *         2 if ack/nack received although the opposite was supposed to happen
 */
int twi_communicate(struct twi_data *data, uint8_t amount, uint8_t *failed_command);

#endif
