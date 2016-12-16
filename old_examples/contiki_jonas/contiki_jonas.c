#include "contiki.h"
#include "contiki_jonas_sdds_impl.h"
#include <avr/io.h>

PROCESS(contiki_jonas, "Security Example");
AUTOSTART_PROCESSES(&contiki_jonas);

PROCESS_THREAD(contiki_jonas, ev, data)
{
	static struct etimer g_wait_timer;

	PROCESS_BEGIN();

	DDS_ReturnCode_t ret;
  DDS_ParticipantStatelessMessage *ptr;
  DDS_SampleInfo info;
  
  extern unsigned char __heap_start;
  uint16_t momentan_frei = SP - (uint16_t) &__heap_start;
  printf("frei: %d\n", momentan_frei);

  if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  
  static Beta beta_sub;
  Beta *beta_sub_p = &beta_sub;

  for (;;) {
          
    ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader, &beta_sub_p, NULL);

    if (ret == DDS_RETCODE_NO_DATA) {

      printf("no data for beta\n");

    } else {

      printf("Received a sample from topic 'beta': {\n"
        "   value => %c\n"
        "   value2 => %s\n"
        "   value3 => %s\n"
        "}\n"
        , beta_sub_p->value
        , beta_sub_p->value2
        , beta_sub_p->value3);

    }
           
    etimer_set(&g_wait_timer, 1 * CLOCK_SECOND);
    PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));

  }
  

  PROCESS_END();

}




