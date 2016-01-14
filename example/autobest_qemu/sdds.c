
#include <stdio.h>
#include <sdds_sdds_impl.h>
#include <ipc-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>

#include <hv.h>
//#include <trace_api.h>

#define SLEEP_TIMEOUT_NSEC 2000000ULL

int main(void);

int main(void){

	rc_t ret = sDDS_init();
	if(ret != SDDS_RT_OK){
		sys_task_terminate();
    	sys_abort();
		return -1;
	}
	Log_setLvl(0);
	printf("Initilaized sDDS\n");
	Ipc ipc_data_used;

	Ipc data_used;
	Ipc* data_used_ptr = &data_used;

	for (;;){
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
		if (ret == DDS_RETCODE_NO_DATA){
/*#ifndef NDEBUG
			trace_test(AUTOBETS_ETHERNET_IN_EVENT);
#endif*/
			//Thread_sleep(NULL, 1);
			sys_sleep(SLEEP_TIMEOUT_NSEC);
		}
		else{
/*#ifdef FEATURE_SDDS_TRACING_ENABLED
	        Trace_point(SDDS_TRACE_EVENT_STOP);
#endif*/
			//printf("received: %x\n",data_used.value );
		}

	}
	/*for(;;){
		trace_test(5);
	}*/
	sys_task_terminate();
    sys_abort();
	return 0;
}
