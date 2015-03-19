#include "atmega_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/Log.h>
#include <sdds/sdds_types.h>
#include <os-ssal/NodeConfig.h>
#include "SDDS_Application.h"

#include <avr/pgmspace.h>

#include <contiki.h>
#include <contiki-net.h>

#include <avr/eeprom.h>
#include <avr/io.h>

#include "ATMEGA_LED.h"
#include "LED.h"

// workaround to prevent contiki to overwrite the mac adress in eeprom
char atmega128rfa1_macadress[8]       EEMEM;

SSW_NodeID_t nodeID = 0;


#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)



PROCESS(periodicPublishProcess, "Periodic_sdds_publish_process");

//PROCESS(changeRecognitionProcess, "sdds process to recognise changes");

AUTOSTART_PROCESSES(&periodicPublishProcess);

// timer for periodicPublishProcess
static struct etimer g_wait_timer;

// timer for change recofnition process
//static struct etimer g_changeReco_timer;

LED g_statusled;



PROCESS_THREAD(periodicPublishProcess, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	rc_t ret;

	sDDS_init();

	Log_setLvl(0);

	Log_debug("sdds initilised\n");
	

	uint16_t myAddr = (uint16_t) &atmega128rfa1_macadress;

		Log_debug("Addr mac 0x%x\n", myAddr);

		uint8_t byte1 = eeprom_read_byte((uint8_t*) myAddr);

/*
		if (byte1 == 0xff) {
			eeprom_write_byte((uint8_t*)myAddr+7, 0x00);
			eeprom_write_byte((uint8_t*)myAddr+6, 0x21);
			eeprom_write_byte((uint8_t*)myAddr+5, 0x2E);
			eeprom_write_byte((uint8_t*)myAddr+4, 0xFF);
			eeprom_write_byte((uint8_t*)myAddr+3, 0xFF);
			eeprom_write_byte((uint8_t*)myAddr+2, 0x00);
			eeprom_write_byte((uint8_t*)myAddr+1, 0x22);
			eeprom_write_byte((uint8_t*)myAddr, 0xE3);
		}
*/

		for (uint8_t i = 0; i < 8; i++) {
			uint8_t byte = eeprom_read_byte((uint8_t*) myAddr+i);
			Log_debug("byte %d : 0x%2x \n", i, byte);
		}

	nodeID = NodeConfig_getNodeID();
	Log_debug("device id : 0x%X : %d \n", nodeID, nodeID);

	// init sdds application
	ret = SDDS_Application_init();




	// init status led
	// create and init instance

	static struct LED_t statusled_stc = {
			.bank = HAL_LED_BANK_D,
			.pin = HAL_LED_PIN_7,
			.sourceing = false
#ifdef DRIVER_LED_DIMMING			    
			,.mode = 0
#endif
	};
	// file scope var pointer
	g_statusled = &statusled_stc;
	ret = LED_init(g_statusled);

	LED_switchOn(g_statusled);

	// starten der ggf. vorhanden eigenen threads der application

	ret = SDDS_Application_start();

	while (true)
	{
		do
		{

			// dowork methode der sdds application

			ret = SDDS_Application_doWork();

			Log_debug("foo\n");

		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}

