#include "atmega_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>

#include <contiki.h>
#include <contiki-net.h>

PROCESS(write_process, "sDDS");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;







PROCESS_THREAD(write_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;


	sDDS_init();

	for (;;)
	{


		do
		{
			static uint8_t foo = 0;
			Test t;

			t.testvalue = foo++;

			if (DDS_TestDataWriter_write(g_Test_writer, &t, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}
