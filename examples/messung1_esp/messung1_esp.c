#include "espressif/esp_common.h"
#include "espressif/sdk_private.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "ssid_config.h"

#include "messung1_esp_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"

static xQueueHandle mainqueue;
DDS_Topic g_Alpha_topic;
Alpha g_Alpha_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_Alpha_reader;
DDS_DataWriter g_Alpha_writer;

void task_w(void *pvParameters)
{
	DDS_ReturnCode_t ret;

	Alpha a;
	a.value = 0;

	for (;;) {
		ret = DDS_AlphaDataWriter_write(g_Alpha_writer, &a, NULL);

		if (ret != DDS_RETCODE_OK){
			printf("Write error \r\n");
		}

		a.value++;
		vTaskDelay(100);
	}
}

void user_init(void)
{
	sdk_uart_div_modify(0, UART_CLK_FREQ / 115200);
	printf("SDK version:%s\n", sdk_system_get_sdk_version());

	mainqueue = xQueueCreate(10, sizeof(uint32_t));

	struct sdk_station_config config = {
		.ssid = WIFI_SSID,
		.password = WIFI_PASS,
	};

	sdk_wifi_set_opmode(STATION_MODE);
	sdk_wifi_station_set_config(&config);

	vTaskDelay(100);

	sDDS_init();

	xTaskCreate(task_w, (signed char *)"tsk_w", 256, &mainqueue, 2, NULL);
}
