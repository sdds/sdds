#include "experiments-drivers.h"
#include "twi.h"
#include "twi-pcf.h"

#define TWI_TI_MODE1_DATA 0x0
#define TWI_TI_MODE2_DATA 0x0

#if REGISTER_SINGLE
#define TWI_TI_AUTO_INCREMENT 0
#else
#define TWI_TI_AUTO_INCREMENT (1 << 2)
#endif

int init_light(void)
{
#if 0
	// for connection to the adc
	adc_init_mux(ADC_USE_INTERNAL);
	adc_init();
#else
	// for connection to the twi
	twi_init();
#endif

	return 0;
}

int read_light(uint8_t *value)
{
#if 0
	// for connection to the adc input 0
	*value = (adc_read(0) * 256) / 65536;
	return 0;
#else
	// for connection to the pcf channel 0
	if (twi_init_pcf(0) != 0)
		return 1;

	return twi_read_pcf(value);
#endif
}

int swrst_ti(void)
{
	twi_data_t data[4];
	uint8_t failed_command;

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xD6;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xA5;

	data[3].type = TWI_TYPE_TRANSMIT_DATA;
	data[3].byte = 0x5A;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return -1;

	return 0;
}

int setup_ti_one(uint8_t address)
{
	twi_data_t data[0x1B + 4];
	twi_data_t *tlc_regs;
	uint8_t i;
	uint8_t failed_command;

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = address;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = (0x0 | (TWI_TI_AUTO_INCREMENT << 5));

	tlc_regs = data + 3;

	tlc_regs[0].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0].byte = (TWI_TI_MODE1_DATA | (TWI_TI_AUTO_INCREMENT << 5));

	tlc_regs[1].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[1].byte = TWI_TI_MODE2_DATA;

	/* Brightness Control Registers */
	for (i = 2; i < 0x12; i++)
	{
		tlc_regs[i].type = TWI_TYPE_TRANSMIT_DATA;
		tlc_regs[i].byte = 0x10;
	}

	/* Group Duty Cycle Control Register */
	tlc_regs[0x12].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x12].byte = 0xFF;

	/* Group Frequency Register */
	tlc_regs[0x13].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x13].byte = 0xFF;

	/* LED Driver Output State Registers */
	for (i = 0; i < 4; i++)
	{
		tlc_regs[0x14 + i].type = TWI_TYPE_TRANSMIT_DATA;
		tlc_regs[0x14 + i].byte = 0xFF;
	}

	/* I2C Bus Subaddress Registers */
	tlc_regs[0x18].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x18].byte = 0xD2;
	tlc_regs[0x19].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x19].byte = 0xD4;
	tlc_regs[0x1A].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x1A].byte = 0xD6;

	/* LED All Call I2C Bus Address Register */
	tlc_regs[0x1B].type = TWI_TYPE_TRANSMIT_DATA;
	tlc_regs[0x1B].byte = 0xD0;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return -1;

	return 0;
}

int setup_ti_all(void)
{
	if (setup_ti_one(TWI_TI0_ADDRESS) != 0)
		return -1;

	if (setup_ti_one(TWI_TI1_ADDRESS) != 0)
		return -1;

	if (setup_ti_one(TWI_TI2_ADDRESS) != 0)
		return -1;

	return 0;
}

int control_ti_led(uint8_t address, uint8_t led_index, uint8_t byte)
{
	twi_data_t data[0x4];
	uint8_t failed_command;

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = address;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = (0x14 + led_index | (0x4 << 5));

	data[3].type = TWI_TYPE_TRANSMIT_DATA;
	data[3].byte = byte;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return -1;

	return 0;
}
