
#include <stdio.h>
#include "sdds_sdds_impl.h"
#include <ipc-ds.h>
//#include <beta-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>
#include <hv.h>

#define SLEEP_TIMEOUT_NSEC 50000000ULL

rc_t
shm_server_init();

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	//Log_setLvl(0);

    Ipc ipc_pub;
    ipc_pub.value = 0xAFFE;

    for (;;) {

        ret = DDS_IpcDataWriter_write (g_Ipc_writer, &ipc_pub, NULL);
#ifdef FEATURE_SDDS_TRACING_ENABLED
        Trace_point(SDDS_TRACE_EVENT_DUMMY_1);
        Trace_point(SDDS_TRACE_EVENT_DUMMY_2);
#ifdef FEATURE_SDDS_TRACING_SEND_ISOLATED
        Trace_point(SDDS_TRACE_EVENT_DUMMY_3);
        Trace_point(SDDS_TRACE_EVENT_DUMMY_4);
        Trace_point(SDDS_TRACE_EVENT_DUMMY_5);
        Trace_point(SDDS_TRACE_EVENT_DUMMY_6);
        Trace_point(SDDS_TRACE_EVENT_DUMMY_7);
#endif
        Trace_point(SDDS_TRACE_EVENT_STOP);
#endif
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic ipc\n");
        }
        sys_sleep(SLEEP_TIMEOUT_NSEC);
    }

    sys_task_terminate();
    sys_abort();
	return 0;
}

rc_t
shm_server_init(){
    return SDDS_RT_OK;
}
