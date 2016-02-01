
#include <stdio.h>
#include "sdds_app2_sdds_impl.h"
#include <alpha-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>
#include <string.h>

#include <hv.h>
//#include <trace_api.h>

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

	Alpha data_used;
	Alpha* data_used_ptr = &data_used;

	for (;;){
		DDS_ReturnCode_t ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &data_used_ptr, NULL);
		if (ret == DDS_RETCODE_OK){
            printf("received Alpha: %x\n", (unsigned char)data_used.value );
		}
        sys_sleep(SLEEP_TIMEOUT_NSEC);
	}
    sys_task_terminate();
    sys_abort();
	return 0;
}
