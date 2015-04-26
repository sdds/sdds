#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>
#include <os-ssal/NodeConfig.h>
#include "atmega_sdds_impl.h"

#include <contiki.h>
#include <clock.h>

#include "MovementSensor-ds.h"

#include "AMN31112.h"
#include "GPIO_Input.h"
#include "LED.h"
#include "ATMEGA_LED.h"

#define WIEDAS_MOVEMENT_LED

#define MOVEMENT_isMoving 0
#define MOVEMENT_notMoving 1

#define MOVEMENT_EVENT_MOVEING 1

static struct etimer movement_detection_timer;

#if 1

PROCESS(wiedas_movement_callback_handler, "WieDAS Movement IRQ handler");

extern SSW_NodeID_t nodeID;

static GPIO_Input g_gpio_PIR;
static LED g_move_LED;

void PIR_CallBack_handler( GPIO_Input _this, bool_t state);

rc_t _publishMovement(bool_t value);

rc_t Wiedas_SensorApp_Movement_init() {

	rc_t ret;

	// init led green for movement indicator
#ifdef WIEDAS_MOVEMENT_LED

	static struct LED_t led_stc = {
			.bank = HAL_LED_BANK_B,
			.pin = HAL_LED_PIN_6,
			.sourceing = false,
#ifdef DRIVER_LED_DIMMING
			.resolution = HAL_LED_DIM_RESOLUTION_10BIT,
			.mode = (HAL_LED_DIM_MODE_FAST_PWM | HAL_LED_DIM_ACTIVATE),
			.dimValue = 90
#endif 
	};
	g_move_LED = &led_stc;
	ret = LED_init(g_move_LED);

	LED_switchOff(g_move_LED);
#ifdef DRIVER_LED_DIMMING
	LED_dim(g_move_LED, 90);
#endif

#endif
	//ret = AMN31112_init();

	// use gpio input as driver
	static struct GPIO_Input_t pir_input = {
		.pin = 0,
		.bank = DRIVER_GPIO_INPUT_ATMEGA_BANK_B,
		.mode = DRIVER_GPIO_INPUT_MODE_INTERUPT_ENABLE
	};
	g_gpio_PIR = &pir_input;

	ret = GPIO_Input_init(&pir_input);

	if (ret != SDDS_RT_OK) {
		Log_error("Can't init PIR movement sensor\n");
		return ret;
	}

	// register callback
	ret = GPIO_Input_setCallback(g_gpio_PIR, &PIR_CallBack_handler);
	if (ret != SDDS_RT_OK) {
		Log_error("Can't register PIR movement callback\n");
		return ret;
	}

	// init the callback handler process
	process_start(&wiedas_movement_callback_handler, NULL);




	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Movement_start() {
	return SDDS_RT_OK;
}

rc_t _publishMovement(bool_t value) {
	rc_t ret;

	// publish regular data
		MovementSensor data;

		data.id = nodeID;
		if (value == true) {
			data.state = MOVEMENT_isMoving;

#ifdef WIEDAS_MOVEMENT_LED
			LED_switchOn(g_move_LED);
#endif
		} else {
			data.state = MOVEMENT_notMoving;

#ifdef WIEDAS_MOVEMENT_LED
			LED_switchOff(g_move_LED);
#endif		
		}

		DDS_ReturnCode_t ddsret;

		ddsret = DDS_MovementSensorDataWriter_write(g_MovementSensor_writer, &data, NULL);

		if (ddsret != DDS_RETCODE_OK) {
			return SDDS_RT_FAIL;
		}

		return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Movement_dowork() {


	static uint8_t waitCount = 0;

	if (waitCount < WIEDAS_SENSORAPP_MOVEMENT_INTERVALL) {
		waitCount++;
		return SDDS_RT_OK;
	} else {
		waitCount = 0;
	}

	// read sensor
	bool_t movement = 0;


	rc_t ret;

	ret = GPIO_Input_getState(g_gpio_PIR, &movement);

	Log_debug("checked movement: %d \n", movement);

	return _publishMovement(movement);

}
rc_t Wiedas_SensorApp_Movement_process() {
	return SDDS_RT_OK;
}

void PIR_CallBack_handler( GPIO_Input _this, bool_t state) {

//	uint8_t event = MOVEMENT_EVENT_MOVEING;

	//Log_debug("Something moved state %d \n", state);

	process_post(&wiedas_movement_callback_handler, 23, (void*) _this);

}

PROCESS_THREAD(wiedas_movement_callback_handler, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    PROCESS_WAIT_EVENT();

    do {
    	static bool_t prevalue = 0;



    	GPIO_Input_getState(g_gpio_PIR, &prevalue);

    	GPIO_Input_deactivateInterrupt(g_gpio_PIR);

        etimer_set(&movement_detection_timer, CLOCK_SECOND / 10);

        PROCESS_YIELD_UNTIL(etimer_expired(&movement_detection_timer));

    	bool_t value2;
    	GPIO_Input_getState(g_gpio_PIR, &value2);


    	if (prevalue == value2) {
    		Log_debug("Something moved %d \n", value2);
    		_publishMovement(value2);
    	}

    	GPIO_Input_activateInterrupt(g_gpio_PIR);


    } while (0);

  }

  PROCESS_END();
}

#endif
