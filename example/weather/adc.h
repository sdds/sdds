#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include <stdint.h>

/*
 * analog/digital conversion module
 * --------------------------------
 *
 * first setup the muxer with adc_init_mux
 * then you can call adc_init
 */

/* reference voltage */
enum adc_ref_voltage
{
	ADC_USE_AREF,
	ADC_USE_AVCC,
	ADC_USE_INTERNAL
};

typedef enum adc_ref_voltage adc_ref_voltage_t;

/* 
 * read a specific adc channel
 *
 * @param channel the channel to read from
 *
 * @return the value read
 */
uint16_t adc_read(uint8_t channel);

/* 
 * setup the adc muxing register
 *
 * @param mode the reference voltage
 */
void adc_init_mux(adc_ref_voltage_t mode);

/*
 * intialize the adc
 */
void adc_init(void);

#endif
