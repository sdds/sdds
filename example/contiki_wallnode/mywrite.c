#include "atmega_sdds_impl.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/Log.h>

#include <contiki.h>
#include <contiki-net.h>

#include "AMN31112.h"
#include "TSL2561.h"
#include "ATMEGA_LED.h"
#include "LED.h"
#include "twi.h"


#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

PROCESS(periodicPublishProcess, "Periodic sdds publish process");

PROCESS(changeRecognitionProcess, "sdds process to recognise changes");

AUTOSTART_PROCESSES(&periodicPublishProcess);

// timer for periodicPublishProcess
static struct etimer g_wait_timer;

// timer for change recofnition process
static struct etimer g_changeReco_timer;

static LED statusled;

void _publishTemperatureSensor();
void _publishTemperatureAndHumiditySensor();
void _publishLightSensor();

void _checkMovement();
void _checkDoor();
void _checkSwitch();



PROCESS_THREAD(periodicPublishProcess, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	rc_t ret;

	sDDS_init();
	Log_setLvl(0);
	
	// init temperatur sensor
	// todo temp sensor treiber impl.
	// init luftfeuchtigkeitssensor
	// todo luftfeuchtigkeitssensor impl.

	// init light sensor
	twi_activateInternalPullUp();
	ret = TSL2561_init();
	if (ret != SDDS_RT_OK) {
		Log_error("can't init tsl2561 ret %d\n", ret);
	}
	uint8_t id = 0;
	ret = TSL2561_readID(&id);
	Log_debug("TSL2561: id 0x%x, ret %d\n", id, ret);

	// init PIR Sensor
	ret = AMN31112_init();

	// init door sensors?
	// todo impl switch sensor driver

	// init switch sensors
	// todo impl switch sensor driver

	// init status led
	// create and init instance
	static struct LED_t statusled_stc = {
			.port = &PORTB,
			.ddr = &DDRB,
			.pin = &PINB,
			.p = PB7,
			.sourceing = false
	};
	// file scope var pointer
	statusled = &statusled_stc;
	ret = LED_init(statusled);

	// init RGB LED
	// todo impl RGB LED driver

	// init something to recognise change in door, switch, and movement

	// start recognise process
	process_start(&changeRecognitionProcess, NULL);


	while (true)
	{
		do
		{
			// publish temperatur value
			_publishTemperatureSensor();
			// publish temperature and humidity value
			_publishTemperatureAndHumiditySensor();
			// publish light value
			_publishLightSensor();

		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}

PROCESS_THREAD(changeRecognitionProcess, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	while (true)
	{
		do
		{
			_checkMovement();
			_checkDoor();
			_checkSwitch();

		}while(0);

		etimer_set(&g_changeReco_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_changeReco_timer));
	}

	PROCESS_END();
}

void _checkMovement()
{

	bool_t movement;

	AMN31112_readMovement(&movement);
	printf("checked movement: %d \n", movement);

	if (movement)
	{
		LED_switchOn(statusled);
	} else {
		LED_switchOff(statusled);
	}
}

void _checkDoor()
{

}
void _checkSwitch()
{

}

void _publishTemperatureSensor()
{
	// todo impl tempsensor
}
void _publishTemperatureAndHumiditySensor()
{
	// todo impl humidity sensor
}
void _publishLightSensor()
{
	rc_t ret;

	uint16_t ch0, ch1;
	ret = TSL2561_getChannels(&ch0, &ch1);

	if (ret != SDDS_RT_OK) {
		Log_error("cant read channels of tsl2561 ret %d\n", ret);
		return;
	}
	Log_debug("tsl2561: channel 0 %u channel 1 %u \n", ch0, ch1);

	uint32_t lux = 0;
	ret = TSL2561_calculateLux(ch0, ch1, &lux);

	Log_debug("tsl2561: lux %u 0x%x \n", lux, lux);

}
