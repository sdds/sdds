
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

	Ipc data_used;
	Ipc* data_used_ptr = &data_used;

	for (;;)
	{
		vtimer_usleep(2 *SEC_IN_USEC);
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
		if (ret == DDS_RETCODE_NO_DATA)
		{
			printf("No Data\n");
		}
		else
		{
			printf("received: %d\n ",data_used.value );
		}
	}
}
