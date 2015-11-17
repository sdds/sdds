#include "linux_mcast1_sdds_impl.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Beta beta_pub;

    for (;;) {
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic beta\n");

        sleep (1);
    }

    return 0;
}
