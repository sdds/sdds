#include <unistd.h>
#include "linux_steffen1_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(5);

	TestQosReliabilityBasicReliableAck testQosReliabilityBasicReliableAck_pub;


    for (;;) {
		DDS_TestQosReliabilityBasicReliableAckDataWriter_write (g_TestQosReliabilityBasicReliableAck_writer, &testQosReliabilityBasicReliableAck_pub, NULL);
		//printf("send\n");
        sleep (3);
    }

    return 0;
}
