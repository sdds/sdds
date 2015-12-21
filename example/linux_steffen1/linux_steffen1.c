#include <unistd.h>
#include "linux_steffen1_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);

    Testqosreliabilitybasic testqosreliabilitybasic_pub;
    testqosreliabilitybasic_pub.number = 1;
    Testqosreliabilitysmall testqosreliabilitysmall_pub;
    testqosreliabilitysmall_pub.number = 2;
    Testqosreliabilitybig testqosreliabilitybig_pub;
    testqosreliabilitybig_pub.number = 3;
    Testqosreliabilityhuge testqosreliabilityhuge_pub;
    testqosreliabilityhuge_pub.number = 4;

    for (;;) {
        ret = DDS_TestqosreliabilitybasicDataWriter_write (g_Testqosreliabilitybasic_writer, &testqosreliabilitybasic_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic testqosreliabilitybasic\n");

        ret = DDS_TestqosreliabilitysmallDataWriter_write (g_Testqosreliabilitysmall_writer, &testqosreliabilitysmall_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic testqosreliabilitysmall\n");

        ret = DDS_TestqosreliabilitybigDataWriter_write (g_Testqosreliabilitybig_writer, &testqosreliabilitybig_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic testqosreliabilitybig\n");

        ret = DDS_TestqosreliabilityhugeDataWriter_write (g_Testqosreliabilityhuge_writer, &testqosreliabilityhuge_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic testqosreliabilityhuge\n");

        sleep (1);
    }

    return 0;
}
