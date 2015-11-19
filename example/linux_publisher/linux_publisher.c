#include "linux_publisher_sdds_impl.h"
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Beta beta_pub;
    beta_pub.value = 'H';
    beta_pub.value2[0] = 'H';
    beta_pub.value2[1] = 'W';
    beta_pub.value3[0] = 'H';
    beta_pub.value3[1] = 'W';

    for (;;) {
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic beta\n");

        sleep (1);
    }

    return 0;
}
