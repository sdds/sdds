#include <unistd.h>
#include "linux_alpha_pub_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    static Alpha alpha_pub;
    alpha_pub.pkey = 0;
    strncpy (alpha_pub.value2,   "Es gibt im", 10);
    alpha_pub.value3 = 1;
    alpha_pub.device = NodeConfig_getNodeID(); 

    for (;;) {
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);
        if (ret != DDS_RETCODE_OK) {
            printf ("Failed to send an alpha sample\n");
        }
        else {
            printf ("Send an alpha sample\n");
        }

        alpha_pub.pkey++;
        sleep (1);
    }

    return 0;
}
