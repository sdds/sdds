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

    Testqosreliabilitybasic testqosreliabilitybasic_sub;
    Testqosreliabilitybasic* testqosreliabilitybasic_sub_p = &testqosreliabilitybasic_sub;

    Testqosreliabilitysmall testqosreliabilitysmall_sub;
    Testqosreliabilitysmall* testqosreliabilitysmall_sub_p = &testqosreliabilitysmall_sub;

    Testqosreliabilitybig testqosreliabilitybig_sub;
    Testqosreliabilitybig* testqosreliabilitybig_sub_p = &testqosreliabilitybig_sub;

    Testqosreliabilityhuge testqosreliabilityhuge_sub;
    Testqosreliabilityhuge* testqosreliabilityhuge_sub_p = &testqosreliabilityhuge_sub;

    int countNoData = 0;

    for (;;) {
        ret = DDS_TestqosreliabilitybasicDataReader_take_next_sample (g_Testqosreliabilitybasic_reader, &testqosreliabilitybasic_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {printf("no data basic %d\n", countNoData);}
        ret = DDS_TestqosreliabilitysmallDataReader_take_next_sample (g_Testqosreliabilitysmall_reader, &testqosreliabilitysmall_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {printf("no data small %d\n", countNoData);}
        ret = DDS_TestqosreliabilitybigDataReader_take_next_sample (g_Testqosreliabilitybig_reader, &testqosreliabilitybig_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {printf("no data big %d\n", countNoData);}
        ret = DDS_TestqosreliabilityhugeDataReader_take_next_sample (g_Testqosreliabilityhuge_reader, &testqosreliabilityhuge_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {printf("no data huge %d\n", countNoData);}

        countNoData++;
        sleep (1);
    }

    return 0;
}
