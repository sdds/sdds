#include "spi.h"

#include <avr/io.h>
#include <avr/power.h>

uint8_t g_spsr;

void spi_init(void)
{
	static int init = 0;

	if (init)
		return;

	power_spi_enable();

	DDRE = _BV(DDE0); /* MOSI */
	DDRB = _BV(DDB0); /* SCK */
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);

	init = 1;
}

void spi_set_control(uint8_t byte)
{
	SPCR = byte;
}

uint8_t spi_get_control(void)
{
	return SPCR;
}

void spi_set_data(uint8_t byte)
{
	SPDR = byte;
}

uint8_t spi_get_data(void)
{
	return SPDR;
}

void spi_wait(void)
{
	while ((SPSR & _BV(SPIF)) == 0)
		;

	g_spsr = SPSR;
}
