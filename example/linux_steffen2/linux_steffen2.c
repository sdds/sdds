#include <unistd.h>
#include "linux_steffen2_sdds_impl.h"
#include "unistd.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(5);

	TestQosReliabilityBasicReliableAck testQosReliabilityBasicReliableAck_sub;
	TestQosReliabilityBasicReliableAck* testQosReliabilityBasicReliableAck_sub_p = &testQosReliabilityBasicReliableAck_sub;

	rc_t retBasic = SDDS_RT_FAIL;

    for (;;) {

		retBasic = DDS_TestQosReliabilityBasicReliableAckDataReader_take_next_sample (g_TestQosReliabilityBasicReliableAck_reader, &testQosReliabilityBasicReliableAck_sub_p, NULL);
		if(retBasic == SDDS_RT_OK){
			//printf("received Sample\n");
		}

        sleep (1);
    }

    return 0;
}
