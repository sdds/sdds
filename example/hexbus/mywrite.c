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

PROCESS(write_process, "sDDS");
PROCESS(switch_process, "switch control");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;
static struct etimer g_wait_timer_command;

static SSW_NodeID_t nodeID = 0;


#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

void _publishSwitchState();
void _publishLampState();
void _publishWattage();
void _processRelayControl();
void _processLampControl();

PROCESS_THREAD(switch_process, ev, data)
{
	PROCESS_BEGIN();
	// make gcc happy
	(void)ev;
	(void)data;


	while (1) {

		_processRelayControl();
		_processLampControl();

	    etimer_set(&g_wait_timer_command, CLOCK_SECOND);
	    PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer_command));
	}

	PROCESS_END();
}



PROCESS_THREAD(write_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;


	sDDS_init();

	nodeID = NodeConfig_getNodeID();

	Log_setLvl(0);


	process_start(&switch_process, NULL);

	for (;;)
	{


		do
		{
			static char t = 0;

			if (t == 0) {
				_publishWattage();
				t =1;
			} else {

				_publishLampState();

				t=0;
			}

		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}

void _processRelayControl() {

	Switch_control switchControl;
	Switch_control* switchControl_ptr = &switchControl;

	DDS_ReturnCode_t ret = DDS_Switch_controlDataReader_take_next_sample(
			g_Switch_control_reader, &switchControl_ptr, NULL);

	if (ret == DDS_RETCODE_NO_DATA) {
		return;
	}
	if (ret != DDS_RETCODE_OK) {
		Log_error("Error reading topic Switch_control\n");
		return;
	}

	Log_debug("Got a switch command %d for switch 0x%4x\n",
			switchControl.command, switchControl.id);

	if (nodeID == switchControl.id) {
		Log_debug("It's for me!!\n");
		switch (switchControl.command) {
		case (0): // switch off
			Log_debug("switching relay off\n");
			relay_on();
			break;
		case (1): // switch on
			Log_debug("switching relay on\n");
			relay_off();
			break;
		case (2):
			Log_debug("toggle the relay\n");
			relay_toggle();
			break;
		case (3):
			Log_debug("Publish the relay state\n");
			_publishSwitchState();
			break;
		}
	}

}

void _processLampControl() {
	Lamp_control lampControl;
	Lamp_control* lampControl_ptr = &lampControl;

	DDS_ReturnCode_t ret = DDS_Lamp_controlDataReader_take_next_sample(
			g_Lamp_control_reader, &lampControl_ptr, NULL);

	if (ret == DDS_RETCODE_NO_DATA) {
		return;
	}
	if (ret != DDS_RETCODE_OK) {
		Log_error("Error reading topic Lamp_control\n");
		return;
	}

	Log_debug("Got a lamp command %d for lamp 0x%4x\n",
			lampControl.command, lampControl.id);

	if (nodeID == lampControl.id) {
		Log_debug("It's for me!!\n");
		switch (lampControl.command) {
		case (0): // lamp off
			Log_debug("switching lamp off\n");
			relay_on();
			break;
		case (1): // switch on
			Log_debug("switching lamp on\n");
			relay_off();
			break;
		case (2):
			Log_debug("toggle the lamp state\n");
			relay_toggle();
			break;
		case (3):
			Log_debug("Publish the lamp state\n");
			_publishLampState();
			break;
		}
	}
}

void _publishWattage() {
	uint16_t power;
	Wattage w;

	power = metering_get_power();
	w.id = nodeID;
	w.watt = power;
	Log_debug("write dds data for node 0x%x power %d \n", nodeID, power);

	DDS_ReturnCode_t ret = DDS_WattageDataWriter_write(g_Wattage_writer, &w, NULL);

	if (ret != DDS_RETCODE_OK)
	{
		Log_error("Can't publish wattage \n");
	}
}

void _publishSwitchState() {

	bool_t state = relay_get_state();

	Switch s;

	s.id = nodeID;
	s.dummy = 0;

	if (state == 0) {
		s.state  = 0;
	} else {
		s.state = 1;
	}

	Log_debug("Publish Switch state: %d", s.state);

	DDS_ReturnCode_t ret = DDS_SwitchDataWriter_write(g_Switch_writer, &s, NULL);

	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish switch state \n");
	}


}

void _publishLampState() {
	bool_t state = relay_get_state();

	Lamp l;
	l.id = nodeID;
	l.dummy = 0;

	if (state == 0) {
		l.state = 1;
	} else {
		l.state = 0;
	}

	Log_debug("Publish Lamp state: %d\n", l.state);
	DDS_ReturnCode_t ret = DDS_LampDataWriter_write(g_Lamp_writer, &l, NULL);

	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish lamp state \n");
	}
}
