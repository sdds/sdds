#include <unistd.h>
#include "riot_temperature_pub_sdds_impl.h"

int main()
{
    DDS_ReturnCode_t ret;

    Log_setLvl(4);  // set to 0 to see everything.
    if (sDDS_init() == SDDS_RT_FAIL) {
        return 1;
    }

    static Temperature temperature_pub;
    temperature_pub.device = NodeConfig_getNodeID();
    temperature_pub.value = 21;

    for (;;) {
        ret = DDS_TemperatureDataWriter_write (g_Temperature_writer, &temperature_pub, NULL);
        if (ret != DDS_RETCODE_OK) {
            printf ("Failed to send topic temperature\n");
        }
        else {
            printf ("Send a temperature sample\n");
        }

        sleep (1);
    }

    return 0;
}
