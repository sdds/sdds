#include "atmega_sdds_impl.h"

#include <sdds/sDDS.h>
#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <dds/DDS_DCPS.h>
#include <sdds/Log.h>
#include "SDDS_Application.h"

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

PROCESS(periodicPublishProcess, "Periodic_sdds_publish_process");
AUTOSTART_PROCESSES(&periodicPublishProcess);

// timer for periodicPublishProcess
static struct etimer g_wait_timer;


SSW_NodeID_t nodeID = 0;




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


	nodeID = NodeConfig_getNodeID();
	Log_debug("device id : 0x%X : %d", nodeID, nodeID);

	// init sdds application
	ret = SDDS_Application_init();


	// starten der ggf. vorhanden eigenen threads der application

	ret = SDDS_Application_start();

	while (true)
	{
		do
		{

			// dowork methode der sdds application

			ret = SDDS_Application_doWork();


		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}

