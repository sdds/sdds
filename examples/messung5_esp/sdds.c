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
