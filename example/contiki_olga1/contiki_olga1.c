#include "contiki.h"
#include "contiki_olga1_sdds_impl.h"

PROCESS(contiki_olga1, "Empfange Beta-Topic");
AUTOSTART_PROCESSES(&contiki_olga1);

#include "TSL2561.h"
#include "twi.h"
#include "ATMEGA_LED.h"

static LED r_LED;
static LED g_LED;
static LED b_LED;

static Red red_sub;
Red *red_sub_p = &red_sub;

static Green green_sub;
Green *green_sub_p = &green_sub;

static Blue blue_sub;
Blue *blue_sub_p = &blue_sub;

static rc_t
s_init_drivers() {
      rc_t ret;
      ret = TSL2561_init();
      if (ret != SDDS_RT_OK) {
              Log_error("can't init tsl2561 ret %d\n", ret);
              return ret;
      }
      uint8_t id = 0;
      ret = TSL2561_readID(&id);
      Log_debug("TSL2561: id 0x%x, ret %d\n", id, ret);

      static struct LED_t r_led_stc = {
                      .bank = HAL_LED_BANK_B,
                      .pin = HAL_LED_PIN_5,
                      .sourceing = false,
                      .resolution = HAL_LED_DIM_RESOLUTION_10BIT,
                      .mode = (HAL_LED_DIM_MODE_FAST_PWM | HAL_LED_DIM_ACTIVATE),
                      .dimValue = 0

      };

      static struct LED_t b_led_stc = {
                      .bank = HAL_LED_BANK_B,
                      .pin = HAL_LED_PIN_7,
                      .sourceing = false,
                      .resolution = HAL_LED_DIM_RESOLUTION_10BIT,
                      .mode = (HAL_LED_DIM_MODE_FAST_PWM | HAL_LED_DIM_ACTIVATE),
                      .dimValue = 0

      };

      r_LED = &r_led_stc;
      b_LED = &b_led_stc;

      ret = LED_init(r_LED);
      ret = LED_init(b_LED);

      LED_switchOff(r_LED);
      LED_switchOff(b_LED);

      LED_dim(r_LED, 0);
      LED_dim(b_LED, 0);
}

PROCESS_THREAD(contiki_olga1, ev, data)
{
	static struct etimer g_wait_timer;

	PROCESS_BEGIN();

    printf("driver\n");
    s_init_drivers();
    printf("driver initialised\n");

//	DDS_ReturnCode_t ret;
//
//	if (sDDS_init() == SDDS_RT_FAIL) {
//		return 1;
//	}
	Log_setLvl(0);

    for (;;) {
//        ret = DDS_RedDataReader_take_next_sample(g_Red_reader, &red_sub_p, NULL);
//        if (ret == DDS_RETCODE_NO_DATA) {
//            printf("no data for red\n");
//        }
//        else {
//            printf("Received a sample from topic 'red': {\n"
//                   "   value => %c\n"
//                   "}\n"
//                   , red_sub_p->value);
//        }
//
//        ret = DDS_GreenDataReader_take_next_sample(g_Green_reader, &green_sub_p, NULL);
//        if (ret == DDS_RETCODE_NO_DATA) {
//            printf("no data for green\n");
//        }
//        else {
//            printf("Received a sample from topic 'green': {\n"
//                   "   value => %c\n"
//                   "}\n"
//                   , green_sub_p->value);
//        }
//
//        ret = DDS_BlueDataReader_take_next_sample(g_Blue_reader, &blue_sub_p, NULL);
//        if (ret == DDS_RETCODE_NO_DATA) {
//            printf("no data for blue\n");
//        }
//        else {
//            printf("Received a sample from topic 'blue': {\n"
//                   "   value => %c\n"
//                   "}\n"
//                   , blue_sub_p->value);
//        }
//

//        uint8_t id = 0;
//        rc_t ret = TSL2561_readID(&id);
//        Log_debug("TSL2561: id 0x%x, ret %d\n", id, ret);

		etimer_set(&g_wait_timer, 1 * CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
    }

    PROCESS_END();
}
