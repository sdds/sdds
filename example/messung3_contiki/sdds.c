#include "contiki.h"
#include "atmega_sdds_impl.h"
#include "Log.h"

/*---------------------------------------------------------------------------*/
PROCESS(messung1_contiki, "Messung 2 Contiki");
AUTOSTART_PROCESSES(&messung1_contiki);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(messung1_contiki, ev, data)
{
	static struct etimer g_wait_timer;

	PROCESS_BEGIN();

	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Alpha alpha_data_used;
	Alpha* alpha_data_used_ptr = &alpha_data_used;

	Beta b;
	b.value = 0;

	while (1) {

		do {
			ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader,
					&alpha_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data alpha\n");
			} else {
				printf("Received (alpha): %d\n", alpha_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		b.value++;

		etimer_set(&g_wait_timer, 10 * CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
