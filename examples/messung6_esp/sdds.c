#include "espressif/esp_common.h"
#include "espressif/sdk_private.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "esp_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"

void task1(void *pvParameters)
{
	DDS_ReturnCode_t ret;

	Alpha a;
	a.value = 0;

	Beta b;
	b.value = 0;

	Gamma gamma_data_used;
	Gamma* gamma_data_used_ptr = &gamma_data_used;


	Delta delta_data_used;
	Delta* delta_data_used_ptr = &delta_data_used;


	while (1) {

		if (DDS_AlphaDataWriter_write(g_Alpha_writer, &a,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		a.value++;

		if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		b.value++;

		do {
			ret = DDS_GammaDataReader_take_next_sample(g_Gamma_reader,
					&gamma_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data gamma\n");
			} else {
				printf("Received (gamma): %d\n", gamma_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		do {
			ret = DDS_DeltaDataReader_take_next_sample(g_Delta_reader,
					&delta_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data delta\n");
			} else {
				printf("Received (delta): %d\n", delta_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		vTaskDelay(10);
	}

}

static xQueueHandle mainqueue;

void user_init(void)
{
	sDDS_init();
	Log_setLvl(0);

    sdk_uart_div_modify(0, UART_CLK_FREQ / 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    mainqueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(task1, (signed char *)"tsk1", 256, &mainqueue, 2, NULL);
}
