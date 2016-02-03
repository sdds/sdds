
#include <stdio.h>
#include "sdds_app1_sdds_impl.h"
#include <ipc-ds.h>
#include <beta-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>
#include <string.h>

#include <hv.h>

#define SLEEP_TIMEOUT_NSEC 1000000000ULL

int main(void);

static addr_t base;
static size_t size;

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

    Beta beta_data;
    beta_data.value = 0xDE;
    memcpy(beta_data.value2, "Der", 4);
    memcpy(beta_data.value3, "Tod", 4);

	for (;;){
		DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
		if (ret == DDS_RETCODE_OK){
            printf("received Ipc: 0x%04x\n", data_used.value );
		}
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
