
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

void
sdds_callback_sdds_app1_ipc(DDS_DataReader reader);

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
#ifdef FEATURE_SDDS_TRACING_RPC_TASK
    Trace_point(SDDS_TRACE_EVENT_RPC_TASK);
#endif
    switch(dds_obj_id){
        case DDS_IPC_READER_ID:
            // check if we want to register a listenr or want to take the next sample from the smaple history.
            if(g_shm_sdds_app1->ipc_seg.type == 0){
                Ipc* data_used_ptr = &g_shm_sdds_app1->ipc_seg.data;
                DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
                Trace_point(SDDS_TRACE_EVENT_RPC_REPLY);
#endif
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }else if(g_shm_sdds_app1->ipc_seg.type == 2){
                //TODO check which listener should be registet
                struct DDS_DataReaderListener listStruct = { .on_data_available =
            			&sdds_callback_sdds_app1_ipc};
                DDS_ReturnCode_t ret = DDS_DataReader_set_listener(g_Ipc_reader, &listStruct, NULL);
#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
                Trace_point(SDDS_TRACE_EVENT_RPC_REPLY);
#endif
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }
            //TODO: if sdds implment more function of the dds standart then we need to check more types for the DataReader.
            break;
    }
#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
    Trace_point(SDDS_TRACE_EVENT_RPC_REPLY);
#endif
    sys_rpc_reply(reply_id, (unsigned long)-1, 1);
    /* if RPC reply returns, the caller's partition was rebooted */
    sys_task_terminate();
	assert(0);
}

void
sdds_callback_sdds_app1_ipc(DDS_DataReader reader){
    // reste status
#ifdef FEATURE_SDDS_TRACING_SERVER_LISTNER
    Trace_point(SDDS_TRACE_EVENT_SERVER_LISTNER);
#endif
    g_shm_sdds_app1->cbs.status = 0;
    g_shm_sdds_app1->cbs.events[g_shm_sdds_app1->cbs.write] = DDS_IPC_READER_ID;
    g_shm_sdds_app1->cbs.write = (g_shm_sdds_app1->cbs.write + 1) % PLATFORM_AUTOBEST_CALLBACK_EVENT_BUF_SIZE;
    // check if we have an overflow and signal it
    if(g_shm_sdds_app1->cbs.write == g_shm_sdds_app1->cbs.read){
        g_shm_sdds_app1->cbs.status |= EVENT_OVERWIEW_FLAG;
    }
#ifdef FEATURE_SDDS_TRACING_IPEV_LISTNER
        Trace_point(SDDS_TRACE_EVENT_IPEV_LISTNER);
#endif
    unsigned int err = sys_ipev_set(CFG_IPEV_reader_callback_sdds_app1);
    assert (err == E_OK);
}

