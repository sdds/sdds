#include <unistd.h>
#include "linux_temperature_sub_polling_sdds_impl.h"

int main()
{
    DDS_ReturnCode_t ret;

    Log_setLvl(4);  // set to 0 for to see everything.
    if (sDDS_init() == SDDS_RT_FAIL) {
        return 1;
    }

    static Temperature temperature_sub;
    Temperature *temperature_sub_p = &temperature_sub;

    for (;;) {
        ret = DDS_TemperatureDataReader_take_next_sample(g_Temperature_reader,
                &temperature_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for temperature\n");
        }
        else {
            printf("Received a sample from topic 'temperature': {\n"
                   "   device => %04X\n"
                   "   value => %"PRId16"\n"
                   "}\n"
                   , temperature_sub_p->device
                   , temperature_sub_p->value);
        }

        sleep (1);
    }

    return 0;
}
