#include "linux_subscriber_sdds_impl.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Ipc ipc_sub;
    Ipc *ipc_sub_p = &ipc_sub;

    for (;;) {
        ret = DDS_IpcDataReader_take_next_sample(g_Ipc_reader,
                &ipc_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for ipc\n");
        }
        else {
            printf("Received (ipc)\n");
        }

        sleep (1);
    }

    return 0;
}
