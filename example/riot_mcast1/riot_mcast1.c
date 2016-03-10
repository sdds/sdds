#include <unistd.h>
#include "riot_mcast1_sdds_impl.h"
#include "unistd.h"

int main()
{
	DDS_ReturnCode_t ret;
	Log_setLvl(1);

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

    Beta beta_pub;
    beta_pub.value = 'H';
    strncpy (beta_pub.value2,   "Es gibt im", 10);
    strncpy (beta_pub.value3,   "Es gibt i", 9);

    for (;;) {
        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        sleep (5);
    }

    return 0;
}
