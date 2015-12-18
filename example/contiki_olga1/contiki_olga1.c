#include "contiki.h"
#include "contiki_olga1_sdds_impl.h"

PROCESS(contiki_olga1, "Empfange Beta-Topic");
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

    Beta beta_pub;
    beta_pub.value = 'H';
    strncpy (beta_pub.value2,   "Es gibt im", 10);
    strncpy (beta_pub.value3,   "Es gibt i", 9);

    for (;;) {
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic beta\n");

		etimer_set(&g_wait_timer, 10 * CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
    }

    PROCESS_END();
}
