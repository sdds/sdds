#include "contiki.h"
#include "contiki_olga1_sdds_impl.h"

PROCESS(contiki_olga1, "    sende green Topic


-->");
AUTOSTART_PROCESSES(&contiki_olga1);

PROCESS_THREAD(contiki_olga1, ev, data)
{
	static struct etimer g_wait_timer;

	PROCESS_BEGIN();

	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);

    static Green green_pub;
    green_pub.value = 'H';

    for (;;) {
        ret = DDS_GreenDataWriter_write (g_Green_writer, &green_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic green\n");

		etimer_set(&g_wait_timer, 10 * CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
    }

    PROCESS_END();
}
