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
#include "LED.h"


#include "atmega_sdds_impl.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "contiki.h"

#define BATTERY_INA_COUNT 1
extern SSW_NodeID_t nodeID;
extern LED g_statusled;
/*  
volatile static uint8_t adc_data;
ISR(ADC_vect) //ADC End of Conversion interrupt
{
	adc_data = ADC >> 2; //read 8 bit value
}
*/
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
//			_BV(ADIE) |
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

	static	bool_t lowBattery = false;
	static uint8_t inactiveCount = 0;


	// check only all BATTERY_INA_COUNT seconds
	// or if the battery is low, than do it every second
	if (lowBattery == false && inactiveCount < BATTERY_INA_COUNT) {
		inactiveCount++;

		return SDDS_RT_OK;
	} else {
		inactiveCount = 0;
	}


	uint16_t sum = 0;

//	clock_time_t t_start = 	clock_time();

	for (uint8_t i = 0; i < 10; i++) {

		// start conversion
		ADCSRA |= _BV(ADSC);

		while (ADCSRA & _BV(ADSC) ) {   // auf Abschluss der Konvertierung warten
  		}

		sum +=	ADC >> 2;
	}
//	Log_debug("sum %d\n", sum);

	uint8_t adc_data = (uint8_t) (sum / 10);

//	clock_time_t t_end = clock_time();

//	Log_debug("Measure time t1 %d t2 %d   d_t %d \n",t_start, t_end, (t_end - 
//	t_start) );

	//get last result
	// calculate value
	// value = 1,8/255 * adc_val
	// since V_ref it 1,8V volt_val is a problem
	//
	
	uint8_t batteryVolt = (uint8_t) ((uint16_t)(71 * adc_data) / 100) ;


	if (batteryVolt < 180) {
		// low battery
		lowBattery = true;
		Log_warn("Battery is below 1,8V! It has %u V/100 \n ", batteryVolt);
		// publish data
		//
		// toggle status led
		LED_toggle(g_statusled);
	} else  {
		Log_debug("Battery voltage is >  %u V/100 \n", batteryVolt );
	}

	if (batteryVolt < 172 ) {
	//	publish shutdown
		
		// shutdown

	}


	return SDDS_RT_OK;
}
