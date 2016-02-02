#include "contiki.h"
#include "contiki_demopub2_sdds_impl.h"

PROCESS(contiki_demoPub2, "Sende green-Topic");
AUTOSTART_PROCESSES(&contiki_demoPub2);

#include "ATMEGA_LED.h"
#include <avr/io.h>

static LED g_LED;


static rc_t
s_init_drivers() {
    static rc_t ret;
      // Set GPIO-pin PE4 to input for Giesomat
      DDRE &= ~(1 << PE4);

      static struct LED_t g_led_stc = {
                      .bank = HAL_LED_BANK_B,
                      .pin = HAL_LED_PIN_6,
                      .sourceing = false,
                      .resolution = HAL_LED_DIM_RESOLUTION_10BIT,
                      .mode = (HAL_LED_DIM_MODE_FAST_PWM | HAL_LED_DIM_ACTIVATE),
                      .dimValue = 0

      };

      g_LED = &g_led_stc;

      ret = LED_init(g_LED);
      LED_switchOff(g_LED);
      LED_dim(g_LED, 150);

      return SDDS_RT_OK;
}

int getValue(){
    static clock_time_t start;
    static clock_time_t stop;

    start = clock_time();

    for (int i = 0; i<10000; i++){
        // wait for PINE4 to get low again
        while( (PINE & (1 << PE4)) );
        // wait until PINE4 is high
        while( !(PINE & (1 << PE4)) );
        // Resetting the watchdog Timer
    }

    stop = clock_time();

    return stop - start;
}

#include <avr/eeprom.h>
char atmega128rfa1_macadress[8]     EEMEM;

PROCESS_THREAD(contiki_demoPub2, ev, data)
{
	static struct etimer g_wait_timer;
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

    s_init_drivers();
    printf("driver initialised\n");

	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(1);

	static int value = 0;
	Green green_pub;
    green_pub.value = 0;

    for (;;) {

        value = getValue();
        green_pub.value = value;

        ret = DDS_GreenDataWriter_write (g_Green_writer, &green_pub, NULL);

		etimer_set(&g_wait_timer, (CLOCK_SECOND/100));
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
    }

    PROCESS_END();
}
