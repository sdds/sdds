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
	//TestQosReliabilitySmallReliableAck testQosReliabilitySmallReliableAck_pub;
	//TestQosReliabilityBigReliableAck testQosReliabilityBigReliableAck_pub;
	//TestQosReliabilityHugeReliableAck testQosReliabilityHugeReliableAck_pub;

	//TestQosReliabilityBasicReliableAck testQosReliabilityBasicReliableAck_sub;
	//TestQosReliabilityBasicReliableAck* testQosReliabilityBasicReliableAck_sub_p = &testQosReliabilityBasicReliableAck_sub;
	TestQosReliabilitySmallReliableAck testQosReliabilitySmallReliableAck_sub;
	TestQosReliabilitySmallReliableAck* testQosReliabilitySmallReliableAck_sub_p = &testQosReliabilitySmallReliableAck_sub;
	//TestQosReliabilityBigReliableAck testQosReliabilityBigReliableAck_sub;
	//TestQosReliabilityBigReliableAck* testQosReliabilityBigReliableAck_sub_p = &testQosReliabilityBigReliableAck_sub;
	//TestQosReliabilityHugeReliableAck testQosReliabilityHugeReliableAck_sub;
	//TestQosReliabilityHugeReliableAck* testQosReliabilityHugeReliableAck_sub_p = &testQosReliabilityHugeReliableAck_sub;


	rc_t retBasic = SDDS_RT_FAIL;
	rc_t retSmall = SDDS_RT_FAIL;
	rc_t retBig = SDDS_RT_FAIL;
	rc_t retHuge = SDDS_RT_FAIL;

	//((Reliable_DataWriter_t*)g_TestQosReliabilityBasicReliableAck_writer)->seqNr = (16-2);
	//((Reliable_DataWriter_t*)g_TestQosReliabilitySmallReliableAck_writer)->seqNr = (256-3);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityBigReliableAck_writer)->seqNr = (65536-4);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityHugeReliableAck_writer)->seqNr = (4294967296-5);

    for (;;) {
		DDS_TestQosReliabilityBasicReliableAckDataWriter_write (g_TestQosReliabilityBasicReliableAck_writer, &testQosReliabilityBasicReliableAck_pub, NULL);
		//DDS_TestQosReliabilitySmallReliableAckDataWriter_write (g_TestQosReliabilitySmallReliableAck_writer, &testQosReliabilitySmallReliableAck_pub, NULL);
		//DDS_TestQosReliabilityBigReliableAckDataWriter_write (g_TestQosReliabilityBigReliableAck_writer, &testQosReliabilityBigReliableAck_pub, NULL);
		//DDS_TestQosReliabilityHugeReliableAckDataWriter_write (g_TestQosReliabilityHugeReliableAck_writer, &testQosReliabilityHugeReliableAck_pub, NULL);

		//retBasic = DDS_TestQosReliabilityBasicReliableAckDataReader_take_next_sample (g_TestQosReliabilityBasicReliableAck_reader, &testQosReliabilityBasicReliableAck_sub_p, NULL);
		retSmall = DDS_TestQosReliabilitySmallReliableAckDataReader_take_next_sample (g_TestQosReliabilitySmallReliableAck_reader, &testQosReliabilitySmallReliableAck_sub_p, NULL);
		//retBig = DDS_TestQosReliabilityBigReliableAckDataReader_take_next_sample (g_TestQosReliabilityBigReliableAck_reader, &testQosReliabilityBigReliableAck_sub_p, NULL);
		//retHuge = DDS_TestQosReliabilityHugeReliableAckDataReader_take_next_sample (g_TestQosReliabilityHugeReliableAck_reader, &testQosReliabilityHugeReliableAck_sub_p, NULL);


        sleep (1);
    }

    return 0;
}
