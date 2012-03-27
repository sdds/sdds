#include "twi.h"
#include "twi-config.h"

#include <avr/io.h>
#include <avr/power.h>

#include <util/twi.h>

/* macros to create easy access to the pin registers */
#define INTERNAL_CONCAT2(x, y) x ## y
#define INTERNAL_CONCAT(x, y) INTERNAL_CONCAT2(x, y)

#define TWI_SDA_DDR INTERNAL_CONCAT(DDR, TWI_DATA_PORT)
#define TWI_SDA_DD INTERNAL_CONCAT(DD, INTERNAL_CONCAT(TWI_DATA_PORT, TWI_DATA_PIN))
#define TWI_SDA_PORTR INTERNAL_CONCAT(PORT, TWI_DATA_PORT)
#define TWI_SDA_PORT INTERNAL_CONCAT(P, INTERNAL_CONCAT(TWI_DATA_PORT, TWI_DATA_PIN))
#define TWI_SCL_PINR INTERNAL_CONCAT(PIN, TWI_CLOCK_PORT)
#define TWI_SCL_PIN INTERNAL_CONCAT(TWI_SCL_PINR, TWI_CLOCK_PIN)
#define TWI_SCL_DDR INTERNAL_CONCAT(DDR, TWI_CLOCK_PORT)
#define TWI_SCL_DD INTERNAL_CONCAT(DD, INTERNAL_CONCAT(TWI_CLOCK_PORT, TWI_CLOCK_PIN))
#define TWI_SCL_PORTR INTERNAL_CONCAT(PORT, TWI_CLOCK_PORT)
#define TWI_SCL_PORT INTERNAL_CONCAT(P, INTERNAL_CONCAT(TWI_CLOCK_PORT, TWI_CLOCK_PIN))

/* the last set status register value */
static uint8_t g_twst;

void twi_init(void)
{
#ifndef F_CPU
#error "Please set F_CPU to the frequency of the target system in Hz."
#endif

	static int init = 0;

	if (init)
		return;

	power_twi_enable();

#ifdef USE_DERFMEGA128
	/* deactivate the onboard twi-eeprom */
	DDRD |= _BV(DDD6);
#endif

	/* setup the scl and sda registers */
	TWI_SCL_PORTR |= _BV(TWI_SCL_DD);
	TWI_SDA_PORTR |= _BV(TWI_SDA_DD);

	/* setup frequency and finally enable the twi functions */
	TWSR = 0;
	TWBR = (F_CPU / TWI_BUS_SPEED - 16) / 2;
	TWDR = 0xFF;
	TWCR = _BV(TWEN);

	init = 1;
}

static void twi_set_control(uint8_t byte)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | byte;
}

static void twi_wait(void)
{
	while ((TWCR & _BV(TWINT)) == 0)
		;

	g_twst = TWSR & TW_STATUS_MASK;
}

static void twi_wait_stop(void)
{
	while (TWCR & _BV(TWSTO))
		;
}

/*
 * for the following functions:
 * -1 on error, 1 on lost, 2 on nack/ack, 0 on success
 */
static int twi_transmit_to_slave(twi_data_t *data)
{
	TWDR = data->byte | TW_WRITE;
	twi_set_control(0);
	twi_wait();

	switch (g_twst)
	{
		case TW_MT_ARB_LOST:
			return 1;

		case TW_MT_SLA_NACK:
			return 2;

		case TW_MT_SLA_ACK:
			return 0;
	}

	return -1;
}

static int twi_receive_from_slave(twi_data_t *data)
{
	TWDR = data->byte | TW_READ;
	twi_set_control(0);
	twi_wait();

	switch (g_twst)
	{
		case TW_MR_ARB_LOST:
			return 1;

		case TW_MR_SLA_NACK:
			return 2;

		case TW_MR_SLA_ACK:
			return 0;
	}

	return -1;
}

static int twi_transmit_data(twi_data_t *data)
{
	TWDR = data->byte;
	twi_set_control(0);
	twi_wait();

	switch (g_twst)
	{
		case TW_MT_ARB_LOST:
			return 1;

		case TW_MT_DATA_NACK:
			return 2;

		case TW_MT_DATA_ACK:
			return 0;
	}

	return -1;
}

static int twi_receive_data(twi_data_t *data, uint8_t set_ack)
{
	twi_set_control(0 | (set_ack ? _BV(TWEA) : 0));
	twi_wait();

	switch (g_twst)
	{
		case TW_MR_ARB_LOST:
			return 1;

		case TW_MR_DATA_NACK:
			data->byte = TWDR;
			return 2;

		case TW_MR_DATA_ACK:
			data->byte = TWDR;
			return 0;
	}

	return -1;
}

static int twi_set_start(void)
{
	twi_set_control(_BV(TWSTA));
	twi_wait();

	if (g_twst == TW_MT_ARB_LOST)
		return 1;

	if (g_twst != TW_START && g_twst != TW_REP_START)
		return -1;

	return 0;
}

int twi_communicate(twi_data_t *data, uint8_t amount, uint8_t *failed_command)
{
	uint8_t i;
	int result;

	if (amount == 0)
		return 0;

	for (i = 0; i < amount; i++)
	{
		switch (data[i].type)
		{
			case TWI_TYPE_START:
				result = twi_set_start();
				break;

			case TWI_TYPE_TRANSMIT_ADDRESS:
				result = twi_transmit_to_slave(data + i);
				break;

			case TWI_TYPE_RECEIVE_ADDRESS:
				result = twi_receive_from_slave(data + i);
				break;

			case TWI_TYPE_TRANSMIT_DATA:
				result = twi_transmit_data(data + i);
				break;

			case TWI_TYPE_RECEIVE_DATA:
				if (i != amount - 1)
				{
					result = twi_receive_data(data + i, 1);
				}
				else
				{
					/* no ack wanted */
					result = twi_receive_data(data + i, 0);

					if (result == 0) /* ack received */
						result = 2;
					else if (result == 2)
						result = 0;
				}

				break;

			default:
				result = 0;
				break;
		}

		if (result == 2) /* ack/nack received although not wanted */
			break;

		if (result == 1) /* arb lost */
			break;

		if (result == -1)
			break;
	}

	twi_set_control(_BV(TWSTO));
	twi_wait_stop();

	if (result != 0)
		*failed_command = i;

	return result;
}


