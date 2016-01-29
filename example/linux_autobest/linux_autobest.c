#include "linux_autobest_sdds_impl.h"
#include <unistd.h>

#define SEND_INTERVAL_US 1000u

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	//Log_setLvl(0);

    Ipc ipc_pub;
    ipc_pub.value = 0xAFFE;

    Beta recv_beta;
    Beta* recv_beta_addr = &recv_beta;

    for (;;) {
        ret = DDS_IpcDataWriter_write (g_Ipc_writer, &ipc_pub, NULL);
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic ipc\n");
        }
        DDS_ReturnCode_t ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader, &recv_beta_addr, NULL);
		if (ret == DDS_RETCODE_OK){
            printf("received Beta: %02x %s %s\n", recv_beta.value, recv_beta.value2, recv_beta.value3 );
		}
        //usleep (SEND_INTERVAL_US);
		sleep(1);
    }

    return 0;
}
