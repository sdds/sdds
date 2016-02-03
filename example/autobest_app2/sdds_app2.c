
#include <stdio.h>
#include "sdds_app2_sdds_impl.h"
#include <alpha-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>
#include <string.h>

#include <hv.h>

#define SLEEP_TIMEOUT_NSEC 500000000ULL

int main(void);

void
callback_alpha(DDS_DataReader reader);

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
    struct DDS_DataReaderListener listStruct = { .on_data_available =
            &callback_alpha};
    DDS_ReturnCode_t dds_ret = DDS_DataReader_set_listener(g_Alpha_reader, &listStruct, NULL);

    if(dds_ret == DDS_RETCODE_ERROR){
        printf("unable to set listenr\n");
    }

	for (;;){
        dds_ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &data_used_ptr, NULL);
		if (dds_ret == DDS_RETCODE_OK){
            printf("received Alpha: %x\n", (unsigned char)data_used.value );
		}
        sys_sleep(SLEEP_TIMEOUT_NSEC);
	}
    sys_task_terminate();
    sys_abort();
	return 0;
}

void
callback_alpha(DDS_DataReader reader){
    printf("New Data\n");
}
