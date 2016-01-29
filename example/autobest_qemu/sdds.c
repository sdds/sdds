
#include <stdio.h>
#include <sdds_sdds_impl.h>
#include <ipc-ds.h>
#include <beta-ds.h>
#include <os-ssal/Thread.h>
#include <os-ssal/Trace.h>
#include <Log.h>

#include <hv.h>
#include <sdds_server_shm.h>
#include <dds_obj_ids.h>
//#include <trace_api.h>

#define SLEEP_TIMEOUT_NSEC 2000000ULL

void
sdds_rpc_sdds_app1(unsigned int reply_id, unsigned long dds_obj_id);

struct {
	char s[1024];
} __stack_rpc_sdds_app1 __aligned(16);

DDS_Object dds_object_map [2];

int main(void);

int main(void){
    rc_t ret = sDDS_init();
    if(ret != SDDS_RT_OK){
        sys_task_terminate();
        sys_abort();
        return -1;
    }
	Log_setLvl(0);

    dds_object_map[0] = g_Ipc_reader;
    dds_object_map[1] = g_Beta_writer;

	for (;;){
		sys_sleep(SLEEP_TIMEOUT_NSEC);
	}

	sys_task_terminate();
    sys_abort();
	return 0;
}

void
sdds_rpc_sdds_app1(unsigned int reply_id, unsigned long dds_obj_id){
    switch(dds_obj_id){
        case DDS_IPC_READER_ID:
            if(g_shm_sdds_app1->ipc_seg.type == 0){
                Ipc data_used;
                Ipc* data_used_ptr = &data_used;
                //Ipc* data_used_ptr = &g_shm_sdds_app1->ipc_seg.data;
                DDS_ReturnCode_t ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader, &data_used_ptr, NULL);
                if(ret == DDS_RETCODE_OK){
                    g_shm_sdds_app1->ipc_seg.data = data_used;
                }
                sys_rpc_reply(reply_id, (unsigned long)ret, 1);
                sys_task_terminate();
            	assert(0);
            }else if(g_shm_sdds_app1->ipc_seg.type == 2){
                sys_rpc_reply(reply_id, -2, 1);
                sys_task_terminate();
            	assert(0);
            }
            break;
        case DDS_BETA_WRITER_ID:
            if(g_shm_sdds_app1->beta_seg.type == 1){
                //Ipc data_used;
                //Ipc* data_used_ptr = &data_used;
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

