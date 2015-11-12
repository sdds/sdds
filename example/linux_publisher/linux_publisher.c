#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "linux_publisher_sdds_impl.h"
#include "ipc-ds.h"

int main()
{
	sDDS_init();

	Ipc ipc_data_used;

	for (;;) {
		sleep(2);

		DDS_ReturnCode_t ret = DDS_IpcDataWriter_write(g_Ipc_writer, &ipc_data_used, NULL);

		if (ret != DDS_RETCODE_OK) {
			printf("sending ipc command failed\n");
		}
		else {
			printf( "sent: %d \n", ipc_data_used.value);
		}
		ipc_data_used.value++;
	}
}
