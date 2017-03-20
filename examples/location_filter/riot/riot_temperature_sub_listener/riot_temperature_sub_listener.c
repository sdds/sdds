#include <unistd.h>
#include "riot_temperature_sub_listener_sdds_impl.h"

void
bathListener(DDS_DataReader reader) {
    Temperature temp;
    Temperature *temp_p = &temp;
	DDS_ReturnCode_t ret;

    memset(&temp, 0, sizeof(Temperature));

    ret = DDS_TemperatureDataReader_take_next_sample(reader, &temp_p, NULL);
    if (ret == DDS_RETCODE_NO_DATA) {
        printf("no data for temperature (WITHIN bathroom)\n");
    }
    else {
        printf("Received a sample from topic 'temperature' (WITHIN bathroom): {\n"
               "   value => %u\n"
               "}\n"
               , temp_p->value);
    }
}

int main() {
    DDS_ReturnCode_t ret;

    Log_setLvl(4);  // set to 0 for to see everything.
    if (sDDS_init() == SDDS_RT_FAIL) {
        return 1;
    }

    printf("Termperature Subscriber (bathroom) DeviceID: %x\n", NodeConfig_getNodeID());

    struct DDS_DataReaderListener bathListStruct = {
            .on_data_available = &bathListener
    };
    ret = DDS_DataReader_set_listener(g_filteredTemperatureBathroom_reader, &bathListStruct, NULL);
    if(ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set listener.\n");
        return SDDS_RT_FAIL;
    }

    for (;;) {
        Log_debug("I'm alive.\n");
        sleep (10);
    }

    return EXIT_SUCCESS;
}

