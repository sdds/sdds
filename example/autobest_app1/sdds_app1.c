
#include <stdio.h>
#include "sdds_app1_sdds_impl.h"
#include <ipc-ds.h>
//#include <beta-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>
#include <string.h>

#include <hv.h>

#define SLEEP_TIMEOUT_NSEC 1000000000ULL

int
main(void);

void
callback_ipc(DDS_DataReader reader);

static Ipc data_used;
static Ipc* data_used_ptr = &data_used;

int
main(void){
	rc_t ret = sDDS_init();
	if(ret != SDDS_RT_OK){
		sys_task_terminate();
    	sys_abort();
		return -1;
	}
	Log_setLvl(0);
	printf("Initilaized sDDS\n");

    Beta beta_data;
    beta_data.value = 0xDE;
    memcpy(beta_data.value2, "Der", 4);
    memcpy(beta_data.value3, "Tod", 4);

    struct DDS_DataReaderListener listStruct = { .on_data_available =
            &callback_ipc};
    DDS_ReturnCode_t dds_ret = DDS_DataReader_set_listener(g_Ipc_reader, &listStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        printf("unable to set listenr\n");
    }

	for (;;){
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_data, NULL);
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic beta\n");
        }
        sys_sleep(SLEEP_TIMEOUT_NSEC);
	}
    sys_task_terminate();
    sys_abort();
	return 0;
}

void
callback_ipc(DDS_DataReader reader){
    DDS_IpcDataReader_take_next_sample(reader, &data_used_ptr, NULL);
#ifdef FEATURE_SDDS_TRACING_ENABLED
    Trace_point(SDDS_TRACE_EVENT_STOP);
#endif
    printf("received Ipc: 0x%04x\n", data_used.value );
}
