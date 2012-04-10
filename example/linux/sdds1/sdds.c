
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "linux_sdds_impl.h"
#include "ipc-ds.h"

void* do_receive(void* foo)
{
	Ipc data_used;
	Ipc* data_used_ptr = &data_used;

	for (;;)
	{
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);

		if (ret == DDS_RETCODE_NO_DATA)
		{
			sleep(1);
		}
		else
		{
			printf("received: %d\n ",data_used.value );
		}
	}
}

int main()
{
    	pthread_t t;

	sDDS_init();

	Ipc ipc_data_used;

	pthread_create(&t, NULL, do_receive, NULL);

	for (;;)
	{
		sleep(2);

		DDS_ReturnCode_t ret = DDS_IpcDataWriter_write(g_Ipc_writer, &ipc_data_used, NULL);

		if (ret != DDS_RETCODE_OK)
		{
			printf("sending ipc command failed\n");
		}
		else
		{
			printf( "sent: %d \n", ipc_data_used.value);
		}

		ipc_data_used.value++;
	}

}
