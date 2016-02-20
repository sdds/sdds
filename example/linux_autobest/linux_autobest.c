#include "linux_autobest_sdds_impl.h"
#include <unistd.h>

#define SEND_INTERVAL_US 50000ULL


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
		return -1;
	}
	Log_setLvl(0);
	printf("Initilaized sDDS\n");



    struct DDS_DataReaderListener listStruct = { .on_data_available =
            &callback_ipc};
    DDS_ReturnCode_t dds_ret = DDS_DataReader_set_listener(g_Ipc_reader, &listStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        printf("unable to set listenr\n");
    }
    for(;;);
	return 0;
}

void
callback_ipc(DDS_DataReader reader){
    DDS_IpcDataReader_take_next_sample(reader, &data_used_ptr, NULL);
    printf("received Ipc: 0x%04x\n", data_used.value );
}

/*int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	//Log_setLvl(0);

    Ipc ipc_pub;
    ipc_pub.value = 0xAFFE;

    /*Beta recv_beta;
    Beta* recv_beta_addr = &recv_beta;

    Alpha alpha_pub;
    alpha_pub.value = 0xAA;*

    for (;;) {

        ret = DDS_IpcDataWriter_write (g_Ipc_writer, &ipc_pub, NULL);
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic ipc\n");
        }
        /*usleep (SEND_INTERVAL_US);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic aplha\n");
        }
        DDS_ReturnCode_t ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader, &recv_beta_addr, NULL);
        if (ret == DDS_RETCODE_OK){
            printf("received Beta: 0x%X %s %s\n", (unsigned char)recv_beta.value, recv_beta.value2, recv_beta.value3 );
		}*
        usleep (SEND_INTERVAL_US);
		//sleep(1);
    }

    return 0;
}
*/
