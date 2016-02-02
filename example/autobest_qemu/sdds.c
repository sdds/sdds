
#include <stdio.h>
#include <sdds_sdds_impl.h>
#include <ipc-ds.h>
#include <beta-ds.h>
#include <alpha-ds.h>
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
void
sdds_rpc_sdds_app2(unsigned int reply_id, unsigned long dds_obj_id);

void
sdds_callback_sdds_app2_alpha(DDS_DataReader reader);

struct {
	char s[1024];
} __stack_rpc_sdds_app1 __aligned(16);
struct {
	char s[1024];
} __stack_rpc_sdds_app2 __aligned(16);

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
    switch(dds_obj_id){
        case DDS_IPC_READER_ID:
            if(g_shm_sdds_app1->ipc_seg.type == 0){
                Ipc* data_used_ptr = &g_shm_sdds_app1->ipc_seg.data;
                DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }else if(g_shm_sdds_app1->ipc_seg.type == 2){
                struct DDS_DataReaderListener listStruct = { .on_data_available =
            			&sdds_callback_sdds_app1_ipc};
                DDS_ReturnCode_t ret = DDS_DataReader_set_listener(g_Ipc_reader, &listStruct, NULL);
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }
            break;
        case DDS_BETA_WRITER_ID:
            if(g_shm_sdds_app1->beta_seg.type == 1){
                Ipc* data_used_ptr = &g_shm_sdds_app1->beta_seg.data;
                DDS_ReturnCode_t ret = DDS_IpcDataWriter_write(g_Beta_writer, data_used_ptr, NULL);
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }
            break;
    }
    sys_rpc_reply(reply_id, (unsigned long)-1, 1);
    /* if RPC reply returns, the caller's partition was rebooted */
    sys_task_terminate();
	assert(0);
}
void
sdds_rpc_sdds_app2(unsigned int reply_id, unsigned long dds_obj_id){
    switch(dds_obj_id){
        case DDS_ALPHA_READER_ID:
            if(g_shm_sdds_app2->alpha_seg.type == 0){
                Alpha* data_used_ptr = &g_shm_sdds_app2->alpha_seg.data;
                DDS_ReturnCode_t ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &data_used_ptr, NULL);
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }else if(g_shm_sdds_app2->alpha_seg.type == 2){
                struct DDS_DataReaderListener listStruct = { .on_data_available =
            			&sdds_callback_sdds_app2_alpha};
                DDS_ReturnCode_t ret = DDS_DataReader_set_listener(g_Alpha_reader, &listStruct, NULL);
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }
            break;
    }
    sys_rpc_reply(reply_id, (unsigned long)-1, 1);
    /* if RPC reply returns, the caller's partition was rebooted */
    sys_task_terminate();
	assert(0);
}

void
sdds_callback_sdds_app1_ipc(DDS_DataReader reader){
    unsigned long recv_arg;
    unsigned int err = sys_rpc_call(CFG_RPC_rpc_reader_callback_sdds_app1, DDS_IPC_READER_ID,-1, &recv_arg);
    assert (err == E_OK);
}
void
sdds_callback_sdds_app2_alpha(DDS_DataReader reader){
    unsigned long recv_arg;
    unsigned int err = sys_rpc_call(CFG_RPC_rpc_reader_callback_sdds_app2, DDS_ALPHA_READER_ID,-1, &recv_arg);
    assert (err == E_OK);
}

