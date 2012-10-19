#include "atmega_sdds_impl.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/Log.h>

#include <contiki.h>
#include <contiki-net.h>

#include "AMN31112.h"
#include "TSL2561.h"
#include "bmp085.h"

#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

PROCESS(write_process, "sDDS");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;







PROCESS_THREAD(write_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	PRINTF("TEST OUT \n");

	DDRB |= _BV(PB7);
	// eingang
//	DDRB &= ~_BV(PB5);
	// pullup anschalten
//	PORTB |= _BV(PB5);

	PORTB |= _BV(PB7);




	sDDS_init();
	Log_setLvl(0);
	
	AMN31112_init();

	rc_t ret;
	/*
	ret = bmp085_init();

	if (ret != SDDS_RT_OK) {
			Log_error("cant init bmp085\n");
	}
	*/
	twi_activateInternalPullUp();



	ret = TSL2561_init();

	if (ret != SDDS_RT_OK) {
			Log_error("cant init tsl2561 ret %d\n", ret);
		}




	uint8_t id = 0;
	ret = TSL2561_readID(&id);

	Log_debug("TSL: id 0x%x, ret %d\n", id, ret);


	PORTB &= ~_BV(PB7);
	for (;;)
	{


		do
		{
			static uint8_t foo = 0;
			Test t;

			t.testvalue = foo++;


			uint16_t ch0, ch1;

			ret = TSL2561_getChannels(&ch0, &ch1);
			if (ret != SDDS_RT_OK) {
						Log_error("cant read channels of tsl2561 ret %d\n", ret);
					}

			Log_debug("channel 0 %u channel 1 %u \n", ch0, ch1);
			uint32_t lux = 0;
			TSL2561_calculateLux(ch0, ch1, &lux);

			Log_debug("lux %u 0x%x \n", lux, lux);

/*
			ret = bmp085_read_temperature(&tem);
			if (ret != SDDS_RT_OK) {
				Log_error("cant read temp\n");
			} else {
				Log_debug("temp %d\n", tem);
			}
*/

			/*
			if (DDS_TestDataWriter_write(g_Test_writer, &t, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
			*/
			//Log_debug("Var %d\n", foo);
			bool_t movement;

			AMN31112_readMovement(&movement);

			if (!movement)
			{
			  //  Log_debug("Bewegung\n");
				PORTB |= _BV(PB7);
			} else {
			 //   Log_debug("nuechts\n");
			    PORTB &= ~_BV(PB7);
			}
			
		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}
