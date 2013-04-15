/*
 * Wiedas_SensorApp_Window.c
 *
 *  Created on: 09.04.2013
 *      Author: kai
 */

#include "Wiedas_sensor_apps.h"
#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>

#include "GPIO_Input.h"

#include "atmega_sdds_impl.h"
#include "WindowSensor-ds.h"

#include <contiki.h>
#include <clock.h>





/* enum OpenCloseState { open, closed }*/
#define OpenCloseState_open 0
#define OpenCloseState_closed 1

extern SSW_NodeID_t nodeID;

// es gibt mehrere tür sensoren, aber maximal 3
static struct GPIO_Input_t inputs[3];

PROCESS(wiedas_window_callback_handler, "WieDAS Window IRQ handler");
static struct etimer window_detection_timer;


void Window_CallBack_handler( GPIO_Input _this, bool_t state);

static rc_t _publishWindowState(uint8_t number, bool_t value);


void Window_CallBack_handler( GPIO_Input _this, bool_t state) {

	// give the pointer to the right gpio structure to the process
	//Log_debug("called\n");
	process_post(&wiedas_window_callback_handler, 23, (void*) _this);
}


rc_t Wiedas_SensorApp_Window_init(uint8_t number) {

	if (number == 0) {
		// PE7 INT7
		inputs[number].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[number].pin = 7;
		inputs[number].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[number]);
		GPIO_Input_setCallback(&inputs[number], &Window_CallBack_handler);
	} else if (number == 1) {
		//PE5 INT5
		inputs[number].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[number].pin = 5;
		inputs[number].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[number]);
		GPIO_Input_setCallback(&inputs[number], &Window_CallBack_handler);
	} else if (number == 2) {
		// PE4 INT4
		inputs[number].bank = GPIO_INPUT_ATMEGA_BANK_E;
		inputs[number].pin = 4;
		inputs[number].mode = (GPIO_INPUT_MODE_INTERUPT_ENABLE
							| GPIO_INPUT_MODE_CALLBACK_LEVEL_TRIGGER
							| GPIO_INPUT_MODE_PULLUP_ACTIVATE);

		GPIO_Input_init(&inputs[number]);
		GPIO_Input_setCallback(&(inputs[number]), &Window_CallBack_handler);
	} else {
		return SDDS_RT_BAD_PARAMETER;
	}

	// init the callback handler process
		process_start(&wiedas_window_callback_handler, NULL);

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Window_start(uint8_t number) {
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Window_dowork(uint8_t number) {


	// get right state
	bool_t value;
	GPIO_Input_getState(&inputs[number], &value);

	_publishWindowState(number, value);


	return SDDS_RT_OK;
}


PROCESS_THREAD(wiedas_window_callback_handler, ev, data)
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
        etimer_set(&window_detection_timer, CLOCK_SECOND / 20);

        PROCESS_YIELD_UNTIL(etimer_expired(&window_detection_timer));

    	bool_t value2;
    	GPIO_Input_getState(&inputs[number], &value2);



    	// todo muss ggf hier angepasst werden, abhängig, ob die reedkontakte öffnend oder schließend sind
    	if (prevalue == value2) {
    		//Log_debug("Window %d state changed to %s\n", number, prevalue ? "open" : "closed");
    		_publishWindowState(number, prevalue);
    	} else { //
    		//Log_debug("Window %d state changed to %s\n", number, prevalue ? "open" : "closed");
    		_publishWindowState(number, prevalue);
    	}


    	GPIO_Input_activateInterrupt(&inputs[number]);


    } while (0);

  }

  PROCESS_END();
}

rc_t _publishWindowState(uint8_t number, bool_t value) {

	WindowSensor data;

	// hack to support more than one windowsensor on one node
	// first window sensor have to have number 0 ;)
	data.id = nodeID + number;
	// depends if open or close is true or false ...
	// todo sollte hardcoded confed werden, HIER
	if (value == true) {
		data.state = OpenCloseState_open;
	} else {
		data.state = OpenCloseState_closed;
	}

	Log_debug("Window %d state %s\n", number, value ? "open" : "closed");

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_WindowSensorDataWriter_write(g_WindowSensor_writer, &data, NULL);

	if (ddsret != DDS_RETCODE_OK) {
		return SDDS_RT_FAIL;
	}


	return SDDS_RT_OK;
}
