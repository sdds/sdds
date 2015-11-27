#include <unistd.h>
#include "linux_mcast1_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Beta beta_pub;
    beta_pub.value = 'H';
    strncpy (beta_pub.value2,   "Es gibt im", 10);
    strncpy (beta_pub.value3,   "Es gibt i", 9);

    for (;;) {
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        if (ret != DDS_RETCODE_OK) {
            printf ("Failed to send topic beta\n");
		}

        sleep (1);
    }

    return 0;
}
