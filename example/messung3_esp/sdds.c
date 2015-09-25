#include "espressif/esp_common.h"
#include "espressif/sdk_private.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "esp_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"

#define sleep(x) vTaskDelay(x)

void task1(void *pvParameters)
{
	DDS_ReturnCode_t ret;

	Alpha alpha_data_used;
	Alpha* alpha_data_used_ptr = &alpha_data_used;

	Beta b;
	b.value = 0;

	for (;;) {

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

		sleep(10);
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
