#include "atmega_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>

#include <contiki.h>
#include <contiki-net.h>

#include <avr/eeprom.h>

#include "ATMEGA_LED.h"
#include "LED.h"

PROCESS(write_process, "sDDS");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;

// workaround to prevent contiki to overwrite the mac adress in eeprom
char atmega128rfa1_macadress[8]       EEMEM;


PROCESS_THREAD(write_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;


	sDDS_init();

	LED g_statusled;
	rc_t ret;

	static struct LED_t statusled_stc = {
			.bank = HAL_LED_BANK_D,
			.pin = HAL_LED_PIN_7,
			.sourceing = false
	};
	// file scope var pointer
	g_statusled = &statusled_stc;
	ret = LED_init(g_statusled);


	for (;;)
	{

		LED_switchOn(g_statusled);
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

		LED_switchOff(g_statusled);
	}

	PROCESS_END();
}
