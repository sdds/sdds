
#include <stdio.h>
#include <sdds_sdds_impl.h>
#include <ipc-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>

#include <hv.h>
#include <sdds_server_shm.h>
#include <dds_obj_ids.h>
#include <app.id.h>

#define SLEEP_TIMEOUT_NSEC 10000000000ULL

void
sdds_rpc_sdds_app1(unsigned int reply_id, unsigned long dds_obj_id);


struct {
	char s[1024];
} __stack_rpc_sdds_app1 __aligned(16);

int main(void);

int main(void){
    rc_t ret = sDDS_init();
    if(ret != SDDS_RT_OK){
        sys_task_terminate();
        sys_abort();
        return -1;
    }
	Log_setLvl(3);
	printf("Initilaized sDDS\n");

	sys_task_terminate();
    sys_abort();
	return 0;
}

void
sdds_rpc_sdds_app1(unsigned int reply_id, unsigned long dds_obj_id){
    //select on witch dds reader or writer the rpc ist called

#ifdef FEATURE_SDDS_TRACING_ENABLED
#ifdef FEATURE_SDDS_TRACING_RPC_TASK
    Trace_point(SDDS_TRACE_EVENT_RPC_TASK);
#endif
#endif
    switch(dds_obj_id){
        case DDS_IPC_WRITER_ID:
            if(g_shm_sdds_app1->ipc_seg.type == 1){
                Ipc* data_used_ptr = &g_shm_sdds_app1->ipc_seg.data;
                DDS_ReturnCode_t ret = DDS_IpcDataWriter_write(g_Ipc_writer, data_used_ptr, NULL);
#ifdef FEATURE_SDDS_TRACING_ENABLED
#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
                Trace_point(SDDS_TRACE_EVENT_RPC_REPLY);
#endif
#endif
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }
            //TODO: if in the future sdds implements more functions of the dds standard we need to check more types for the DataWriter.
            break;
    }
#ifdef FEATURE_SDDS_TRACING_ENABLED
#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
    Trace_point(SDDS_TRACE_EVENT_RPC_REPLY);
#endif
#endif
    sys_rpc_reply(reply_id, (unsigned long)-1, 1);
    /* if RPC reply returns, the caller's partition was rebooted */
    sys_task_terminate();
	assert(0);
}


