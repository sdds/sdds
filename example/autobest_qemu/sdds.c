
#include <stdio.h>
#include <sdds_sdds_impl.h>
#include <ipc-ds.h>
#include <os-ssal/Thread.h>
#include <Log.h>

#include <api_calls.h>

#define SLEEP_TIMEOUT_NSEC 2000000

int main(void);
void* do_receive(void* foo);

int main(void)
{


	rc_t ret = sDDS_init();
	if(ret != SDDS_RT_OK){
		sys_task_terminate();
    	sys_abort();
		return -1;
	}
	Log_setLvl(0);
	printf("Initilaized sDDS\n");
	Ipc ipc_data_used;
	/*Thread rec = Thread_create();
	ssw_rc_t ret = Thread_init(rec, do_receive, NULL);
	if(ret != SDDS_RT_OK){
		sys_task_terminate();
		sys_abort();
	}
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


		Thread_sleep(NULL, 2);
	}*/
	do_receive(NULL);
	sys_task_terminate();
    sys_abort();
	return 0;
}


void* do_receive(void* foo)
{
	Ipc data_used;
	Ipc* data_used_ptr = &data_used;

	for (;;)
	{
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL); 

		if (ret == DDS_RETCODE_NO_DATA)
		{
			Thread_sleep(NULL, 2);
		}
		else
		{
			printf("received: %d\n ",data_used.value );
		}
	}
}
