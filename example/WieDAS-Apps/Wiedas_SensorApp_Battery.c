/*
 * Wiedas_SensorApp_Battery.c
 *
 *  Created on: 21.04.2013
 *      Author: kai
 */

#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>


#include "atmega_sdds_impl.h"

#include <avr/io.h>
#include <avr/interrupt.h>


extern SSW_NodeID_t nodeID;

volatile static uint8_t adc_data;

ISR(ADC_vect) //ADC End of Conversion interrupt
{
	adc_data = ADC >> 2; //read 8 bit value
}

rc_t Wiedas_SensorApp_Battery_init() {

	// manuall init of the stuff, there is not enough time for a driver ...
	// besides it's kind of a hack ..

	// batterie spannung hängt an ADC1 und nicht an AREF ..
	// als vergelich geht nur AVDD, was 1,8V hat
	// damit können nurspannungen unter 1,8v gemessen werden ...


	ADMUX |=
			// set AVDD as reference ...
			_BV(REFS0) |
			// set ADC1 as input
			_BV(MUX0);

	ADCSRA |=
			// enable ADC
			_BV(ADEN) |
			// enable IRQ if conversion is ready
			_BV(ADIE) |
			// division factor 128 ?
			_BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2) |
			// start first conversion
			_BV(ADSC);

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Battery_start() {

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Battery_dowork() {


	// start conversion
	ADCSRA |= _BV(ADSC);

	//get last result
	// calculate value
	// volt_val = V_measu * 255 / V_ref
	// since V_ref it 1,8V volt_val is a problem

	float_t test = adc_data * 255 / 1.8;

	Log_debug("Bat: adc_data %d test %f \n", adc_data, test);

	return SDDS_RT_OK;
}
