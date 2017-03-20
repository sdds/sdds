#include <unistd.h>
#include "linux_temperature_sub_listener_sdds_impl.h"

static void
s_temperatureListener(DDS_DataReader reader);

int main() {
    DDS_ReturnCode_t ret;

    Log_setLvl(4);  // set to 0 for to see everything.
    if (sDDS_init() == SDDS_RT_FAIL) {
        return 1;
    }

    DDS_ReturnCode_t dds_ret;
    struct DDS_DataReaderListener temperatureListenerStruct = {
            .on_data_available = &s_temperatureListener
    };
    
    ret = DDS_DataReader_set_listener(g_Temperature_reader, &temperatureListenerStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set temperatureListenerStruct.\n");
        return EXIT_FAILURE;
    }

    for (;;) {
        Log_debug("I'm alive.\n");
        sleep (10);
    }

    return EXIT_SUCCESS;
}

static void
s_temperatureListener(DDS_DataReader reader) {
    DDS_ReturnCode_t ret;
    static Temperature temperature_sub;
    Temperature *temperature_sub_p = &temperature_sub;

    ret = DDS_TemperatureDataReader_take_next_sample(reader, &temperature_sub_p, NULL);
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
}
