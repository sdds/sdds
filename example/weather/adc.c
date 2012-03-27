#include "adc.h"

#include <avr/io.h>
#include <avr/power.h>

uint16_t adc_read(uint8_t channel)
{
	// only set last 4 bits (channel information)
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);

	// start a/d conversion
	ADCSRA |= 1 << ADSC;

	// wait for it to finish
	while (ADCSRA & (1 << ADSC))
		;

	// return value just extracted
	return ADCW;
}

void adc_init_mux(adc_ref_voltage_t mode)
{
	// set the reference voltage
	switch (mode)
	{
		case ADC_USE_INTERNAL:
			ADMUX = (1 << REFS1) | (1 << REFS0);
			break;

		case ADC_USE_AVCC:
			ADMUX = 1 << REFS0;
			break;

		case ADC_USE_AREF:
			ADMUX = 0;
			break;
	}
}

void adc_init(void)
{
	static int init = 0;

	if (init)
		return;

	power_adc_enable();

	/* 
	 * clock is 8MHz, adc clock should be in range 50-200kHz
	 * 8Mhz / 50kHz == 160, 8Mhz / 200kHz == 40
	 * factor 64 = ADPS2 & ADPS1
	 */
	ADCSRA = (1 << ADPS2) | (1 << ADPS1);

	// enable the adc
	ADCSRA |= 1 << ADEN;

	// start a dummy conversion
	ADCSRA |= 1 << ADSC;

	// and wait until it's finished
	while (ADCSRA & (1 << ADSC))
		;
}
