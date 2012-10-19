#include "atmega_sdds_impl.h"

#include <sdds/sDDS.h>
#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <dds/DDS_DCPS.h>
#include <sdds/Log.h>


#include <os-ssal/NodeConfig.h>

#include <contiki.h>
#include <contiki-net.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <dev/watchdog.h>

#include <metering.h>
#include <relay.h>

PROCESS(publish_process, "sdds publish");
PROCESS(subscribe_process, "sdds subscribe");

// the publish process starts the subsciber process, after initialising sdds
AUTOSTART_PROCESSES(&publish_process);

static struct etimer g_wait_timer_subscribe;
static struct etimer g_wait_timer_publish;

static SSW_NodeID_t nodeID = 0;


#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

void _publishPowerMeter();
void _publishPowerMeterAverage();
void _publishOnOffSwitch();
void _publishSimpleLamp();

void _processOnOffFunctionality();
void _processToggleFunctionality();

PROCESS_THREAD(subscribe_process, ev, data)
{
	PROCESS_BEGIN();
	// make gcc happy
	(void)ev;
	(void)data;


	while (1) {

		_processOnOffFunctionality();
		_processToggleFunctionality();

	    etimer_set(&g_wait_timer_subscribe, CLOCK_SECOND);
	    PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer_subscribe));
	}

	PROCESS_END();
}



PROCESS_THREAD(publish_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;


	sDDS_init();

	nodeID = NodeConfig_getNodeID();

	Log_setLvl(0);


	process_start(&subscribe_process, NULL);

	uint8_t foo = 0;

	while (1)
	{


		if ( foo == 0) {
			_publishOnOffSwitch();
			foo = 1;
		} else

		if (foo == 1) {
			_publishPowerMeter();
			foo = 2;
		} else
		if (foo == 2) {
			_publishSimpleLamp();
			foo = 3;
		} else
		if (foo == 3) {
			_publishPowerMeterAverage();
			foo = 0;
		}

		etimer_set(&g_wait_timer_publish, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer_publish));
	}

	PROCESS_END();
}


void _processOnOffFunctionality() {
	OnOffFunctionality func;
	OnOffFunctionality* func_ptr = &func;

	DDS_ReturnCode_t ret;

	ret = DDS_OnOffFunctionalityDataReader_take_next_sample(
			g_OnOffFunctionality_reader, &func_ptr, NULL );

	if (ret == DDS_RETCODE_NO_DATA) {
		return;
	}
	if (ret != DDS_RETCODE_OK) {
		Log_error("Error reading topic OnOffFunctionality\n");
		return;
	}

	Log_debug(
			"Got a OnOffFunctionality command %d for lamp 0x%4x\n", func.command, func.id);

	if (func.id == nodeID) {
		Log_debug("Command is for this node\n");

		switch (func.command) {
		case (0):
			Log_debug("Switching PowerSocket on\n");
			relay_on();
			break;
		case (1):
			Log_debug("Switching PowerSocket off\n");
			relay_off();
			break;
		default:
			Log_error("Received unknown command %d\n", func.command);
			break;
		}
	}

}
void _processToggleFunctionality()
{
	ToggleFunctionality func;
	ToggleFunctionality* func_ptr = &func;

	DDS_ReturnCode_t ret;

	ret = DDS_ToggleFunctionalityDataReader_take_next_sample(
			g_ToggleFunctionality_reader, &func_ptr, NULL );

	if (ret == DDS_RETCODE_NO_DATA) {
		return;
	}
	if (ret != DDS_RETCODE_OK) {
		Log_error("Error reading topic ToggleFunctionality\n");
		return;
	}

	Log_debug("Got a ToggleFunctionality command %d for lamp 0x%4x\n",
			func.command, func.id);

	if (func.id == nodeID) {
		Log_debug("Command is for this node\n");

		switch (func.command) {
		case (0):
			Log_debug("Toggling PowerSocket\n");
			relay_toggle();
			break;
		default:
			Log_error("Received unknown command %d\n", func.command);
			break;
		}
	}
}


void _publishPowerMeter()
{
	uint16_t power;
	PowerMeter p;

	power = metering_get_power();
	p.id = nodeID;
	p.watt = power;
	Log_debug("Publish PowerMeter power %d \n", power);

	DDS_ReturnCode_t ret = DDS_PowerMeterDataWriter_write(g_PowerMeter_writer, &p,
			NULL );

	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish PowerMeter \n");
	}

}
void _publishPowerMeterAverage()
{

}
void _publishOnOffSwitch()
{
	bool_t state = relay_get_state();

	OnOffSwitch sw;
	sw.id = nodeID;

	if (state == 0) {
		sw.state = 1;
	} else {
		sw.state = 0;
	}
	sw.dummy = 0;

	Log_debug("Publish OnOffSwitch state: %d\n", sw.state);
	DDS_ReturnCode_t ret = DDS_OnOffSwitchDataWriter_write(g_OnOffSwitch_writer, &sw, NULL);
	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish OnOffSwitch \n");
	}

}
void _publishSimpleLamp()
{
	bool_t state = relay_get_state();

	SimpleLamp lamp;
	lamp.id = nodeID;

	if (state == 0) {
		lamp.state = 1;
	} else {
		lamp.state = 0;
	}
	lamp.dummy = 0;

	Log_debug("Publish SimpleLamp state: %d\n", lamp.state);
	DDS_ReturnCode_t ret = DDS_SimpleLampDataWriter_write(g_SimpleLamp_writer,
			&lamp, NULL );

	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish SimpleLamp \n");
	}
}
