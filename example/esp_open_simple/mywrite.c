/* Very basic example that just demonstrates we can run at all!
 */
#include "espressif/esp_common.h"
#include "espressif/sdk_private.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mywrite_sdds_impl.h"

void task1(void *pvParameters)
{
	sDDS_init();

    while(1) {
        vTaskDelay(100);
    }
}

static xQueueHandle mainqueue;

void user_init(void)
{
    sdk_uart_div_modify(0, UART_CLK_FREQ / 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());
    mainqueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(task1, (signed char *)"tsk1", 256, &mainqueue, 2, NULL);
}
