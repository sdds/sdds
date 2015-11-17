#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ipv6/multicast/uip-mcast6.h"

#include "Network.h"
#include "BuiltinTopic.h"
#include "Log.h"
#include "Discovery.h"

#include "mywrite_sdds_impl.h"

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
	static Beta b;

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
	eeprom_write_byte((uint8_t*)myAddr, 0xB9);
#endif

	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	memset(&b, 0, sizeof(b));
	b.value = 0;
	b.value2[0] = 'D';
	b.value2[1] = 'e';
	b.value2[2] = 'r';
	b.value2[3] = '\0';
	b.value3[0] = 'B';
	b.value3[1] = 'a';
	b.value3[2] = 'u';
	b.value3[3] = 'm';
	b.value3[4] = '!';
	b.value3[5] = '\0';


	while (1) {
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
