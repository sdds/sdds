/*
 * Wiedas_SensorApp_Door.c
 *
 *  Created on: 08.04.2013
 *      Author: kai
 */

#include "Wiedas_sensor_apps.h"
#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>

#include "GPIO_Input.h"

#include "atmega_sdds_impl.h"
#include "DoorSensor-ds.h"

#include <contiki.h>
#include <clock.h>


/* enum OpenCloseState { open, closed }*/
#define OpenCloseState_open 0
#define OpenCloseState_closed 1


// es gibt mehrere tür sensoren, aber maximal 3
static struct GPIO_Input_t inputs[3];
// hack to reference the device id, array index is the same as the inputs ...
static uint16_t devIDArray[3];

PROCESS(wiedas_door_callback_handler, "WieDAS Door IRQ handler");
static struct etimer door_detection_timer;


void Door_CallBack_handler( GPIO_Input _this, bool_t state);

static rc_t _publishDoorState(uint16_t deviceID , bool_t value);


void Door_CallBack_handler( GPIO_Input _this, bool_t state) {

	// give the pointer to the right gpio structure to the process
	Log_debug("called\n");
	process_post(&wiedas_door_callback_handler, 23, (void*) _this);
}


rc_t Wiedas_SensorApp_Door_init(uint8_t hwPort, uint16_t deviceID) {

	devIDArray[hwPort] = deviceID;

	if (hwPort == 0) {
		// PE7 INT7
		inputs[hwPort].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[hwPort].pin = 7;
		inputs[hwPort].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[hwPort]);
		GPIO_Input_setCallback(&inputs[hwPort], &Door_CallBack_handler);
	} else if (hwPort == 1) {
		//PE5 INT5
		inputs[hwPort].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[hwPort].pin = 5;
		inputs[hwPort].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[hwPort]);
		GPIO_Input_setCallback(&inputs[hwPort], &Door_CallBack_handler);
	} else if (hwPort == 2) {
		// PE4 INT4
		inputs[hwPort].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[hwPort].pin = 4;
		inputs[hwPort].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[hwPort]);
		GPIO_Input_setCallback(&(inputs[hwPort]), &Door_CallBack_handler);
	} else {
		return SDDS_RT_BAD_PARAMETER;
	}

	// init the callback handler process
		process_start(&wiedas_door_callback_handler, NULL);

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Door_start(uint8_t hwPort, uint16_t deviceID) {
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Door_dowork(uint8_t hwPort, uint16_t deviceID) {

	static uint8_t waitCount = 0;

	if (waitCount < WIEDAS_SENSORAPP_DOOR_INTERVALL) {
		waitCount++;
		return SDDS_RT_OK;
	} else {
		waitCount = 0;
	}

	// get right state
	bool_t value;
	GPIO_Input_getState(&inputs[hwPort], &value);

	_publishDoorState(deviceID, value);


	return SDDS_RT_OK;
}


PROCESS_THREAD(wiedas_door_callback_handler, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    PROCESS_WAIT_EVENT();

    do {
    	static bool_t prevalue = 0;

    	GPIO_Input input = (GPIO_Input) data;

    //	Log_debug("called\n");
    	static uint8_t number;

    	// gehe mal davon aus, dass das hier geht
    	for (number = 0; number < 3; number++)  {
    		if (input == &(inputs[number])) {
    			Log_debug("found %d \n", number);
    			break;
    		}
    	}

    	GPIO_Input_deactivateInterrupt(&inputs[number]);

    	GPIO_Input_getState(&inputs[number], &prevalue);


    	// wait for 50ms
        etimer_set(&door_detection_timer, CLOCK_SECOND / 20);

        PROCESS_YIELD_UNTIL(etimer_expired(&door_detection_timer));

    	bool_t value2;
    	GPIO_Input_getState(&inputs[number], &value2);



    	// todo muss ggf hier angepasst werden, abhängig, ob die reedkontakte öffnend oder schließend sind
    	if (prevalue == value2) {
    		//Log_debug("Door %d state changed to %s\n", number, prevalue ? "open" : "closed");
    		_publishDoorState(devIDArray[number], prevalue);
    	} else { //
    		//Log_debug("Door %d state changed to %s\n", number, prevalue ? "open" : "closed");
    		_publishDoorState(devIDArray[number], prevalue);
    	}


    	GPIO_Input_activateInterrupt(&inputs[number]);


    } while (0);

  }

  PROCESS_END();
}

rc_t _publishDoorState(uint16_t deviceID, bool_t value) {

	DoorSensor data;

	// hack to support more than one doorsensor on one node
	// first door sensor have to have number 0 ;)
	data.id = deviceID;
	// depends if open or close is true or false ...
	// todo sollte hardcoded confed werden, HIER
	if (value == true) {
		data.state = OpenCloseState_open;
	} else {
		data.state = OpenCloseState_closed;
	}

	Log_debug("Door 0x%X state %s\n", deviceID, value ? "open" : "closed");

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_DoorSensorDataWriter_write(g_DoorSensor_writer, &data, NULL);

	if (ddsret != DDS_RETCODE_OK) {
		return SDDS_RT_FAIL;
	}


	return SDDS_RT_OK;
}
