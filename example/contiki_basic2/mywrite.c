#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ipv6/multicast/uip-mcast6.h"

#include "Network.h"
#include "BuiltinTopic.h"
#include "Log.h"
#include "Discovery.h"

#include "atmega_sdds_impl.h"

#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include <avr/eeprom.h>
char atmega128rfa1_macadress[8]		EEMEM;

/*---------------------------------------------------------------------------*/
PROCESS(mcast_sink_process, "Multicast Sink");
AUTOSTART_PROCESSES(&mcast_sink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mcast_sink_process, ev, data)
{
	static struct etimer g_wait_timer;

	static Beta beta_data_used;
	static Beta* beta_data_used_ptr = &beta_data_used;

	PROCESS_BEGIN();

#if 0
	uint16_t myAddr = (uint16_t) &atmega128rfa1_macadress;
	uint8_t byte1 = eeprom_read_byte((uint8_t*) myAddr);
	eeprom_write_byte((uint8_t*)myAddr+7, 0x00);
	eeprom_write_byte((uint8_t*)myAddr+6, 0x21);
	eeprom_write_byte((uint8_t*)myAddr+5, 0x2E);
	eeprom_write_byte((uint8_t*)myAddr+4, 0xFF);
	eeprom_write_byte((uint8_t*)myAddr+3, 0xFF);
	eeprom_write_byte((uint8_t*)myAddr+2, 0x00);
	eeprom_write_byte((uint8_t*)myAddr+1, 0x22);
	eeprom_write_byte((uint8_t*)myAddr, 0xC1);
#endif

	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	while (1) {
		do {
			ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader,
					&beta_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data beta\n");
			} else {
				printf("Received (beta):[%d] %s %s\n", beta_data_used.value,
						beta_data_used.value2, beta_data_used.value3);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		etimer_set(&g_wait_timer, 10 * CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
