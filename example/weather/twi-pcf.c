#include "twi-pcf-config.h"
#include "twi-pcf.h"
#include "twi.h"

int twi_init_pcf(uint8_t channel)
{
	twi_data_t data[4];
	uint8_t failed_command;

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = TWI_PCF_8591P_ADDRESS;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = TWI_PCF_8591P_CONTROL | channel;

	data[3].type = TWI_TYPE_TRANSMIT_DATA;
	data[3].byte = TWI_PCF_8591P_TAP;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return 1;

	return 0;
}

int twi_read_pcf(uint8_t *value)
{
	twi_data_t data[4];
	uint8_t failed_command;

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_RECEIVE_ADDRESS;
	data[1].byte = TWI_PCF_8591P_ADDRESS;

	data[2].type = TWI_TYPE_RECEIVE_DATA; /* throw this byte away */

	data[3].type = TWI_TYPE_RECEIVE_DATA;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return 1;

	*value = data[1].byte;

	return 0;
}
