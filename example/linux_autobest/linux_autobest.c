#include "linux_autobest_sdds_impl.h"
#include <unistd.h>

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Ipc ipc_pub;
    ipc_pub.value = 0xAFFE;

    for (;;) {
        ret = DDS_IpcDataWriter_write (g_Ipc_writer, &ipc_pub, NULL);
        if (ret != DDS_RETCODE_OK){
            printf ("Failed to send topic ipc\n");
        }else{
            printf ("Send: %x\n", ipc_pub.value++);
        }
        sleep (1);
    }

    return 0;
}
