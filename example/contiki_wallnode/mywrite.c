#include "atmega_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/Log.h>
#include <sdds/sdds_types.h>
#include <os-ssal/NodeConfig.h>
#include "SDDS_Application.h"

#include <contiki.h>
#include <contiki-net.h>

#include <avr/eeprom.h>

#include "ATMEGA_LED.h"
#include "LED.h"

// workaround to prevent contiki to overwrite the mac adress in eeprom
char atmega128rfa1_macadress[8]       EEMEM;

SSW_NodeID_t nodeID = 0;


#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)



PROCESS(periodicPublishProcess, "Periodic sdds publish process");

//PROCESS(changeRecognitionProcess, "sdds process to recognise changes");

AUTOSTART_PROCESSES(&periodicPublishProcess);

// timer for periodicPublishProcess
static struct etimer g_wait_timer;

// timer for change recofnition process
static struct etimer g_changeReco_timer;

static LED statusled;


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
	
	nodeID = NodeConfig_getNodeID();

	// init sdds application
	ret = SDDS_Application_init();

	// starten der ggf. vorhanden eigenen threads der application

	ret = SDDS_Application_start();

	// init PIR Sensor
//	ret = AMN31112_init();

	// init door sensors?
	// todo impl switch sensor driver

	// init switch sensors
	// todo impl switch sensor driver

	// init status led
	// create and init instance
	static struct LED_t statusled_stc = {
			.bank = LED_CONF_BANK_D,
			.pin = LED_CONF_PIN_7,
			.sourceing = false
	};
	// file scope var pointer
	statusled = &statusled_stc;
	ret = LED_init(statusled);

	// init RGB LED
	// todo impl RGB LED driver

	// init something to recognise change in door, switch, and movement

	// start recognise process
//	process_start(&changeRecognitionProcess, NULL);


	while (true)
	{
		do
		{

			// dowork mehtode der sdds application

			ret = SDDS_Application_doWork();


		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}
/*
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
*/
