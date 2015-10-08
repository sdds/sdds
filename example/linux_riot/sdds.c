
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "riot_sdds_impl.h"
#include "ipc-ds.h"
#include "os-ssal/Thread.h"
#include "vtimer.h"
#include "thread.h"

int main(void);

int main(void)
{
	Log_setLvl(0);

	sDDS_init();
	printf("Initilaized sDDS\n");
	Ipc ipc_data_used;

	for (;;)
	{
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
		vtimer_usleep(2 *SEC_IN_USEC);
	}
}

