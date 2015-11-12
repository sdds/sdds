#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "linux_subscriber_sdds_impl.h"
#include "ipc-ds.h"

int main()
{
	sDDS_init();

	Ipc data_used;
	Ipc* data_used_ptr = &data_used;

	for (;;) {
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);

		if (ret == DDS_RETCODE_NO_DATA) {
            printf ("no data\n");
			sleep(1);
		}
		else {
			printf ("received: %d\n ",data_used.value );
		}
	}
}
