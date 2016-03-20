#include <unistd.h>
#include "linux_steffen2_sdds_impl.h"
#include "unistd.h"
#include <pthread.h>



Reliable_DataWriter_t* writer_basic_p;
Reliable_DataWriter_t* writer_small_p;
Reliable_DataWriter_t* writer_big_p;
Reliable_DataWriter_t* writer_huge_p;
DataReader_t* reader_basic_p;
DataReader_t* reader_small_p;
DataReader_t* reader_big_p;
DataReader_t* reader_huge_p;


void *getKeyboardInput(){
	while (1) {
		scanf("%d", &g_processingNetworkData);

		printf("INFO: Processing of data from nebuffer is set to %d\n", g_processingNetworkData);
	}
}

int main()
{

	g_processingNetworkData = 1;

	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(5);

	pthread_t p1;

	//pthread_create (&p1, NULL, getKeyboardInput, NULL);


	//TestQosReliabilityBasicReliableNack testQosReliabilityBasicReliableNack_pub;
	//TestQosReliabilitySmallReliableNack testQosReliabilitySmallReliableNack_pub;
	//TestQosReliabilityBigReliableNack testQosReliabilityBigReliableNack_pub;
	//TestQosReliabilityHugeReliableNack testQosReliabilityHugeReliableNack_pub;

	TestQosReliabilityBasicReliableAck testQosReliabilityBasicReliableAck_sub;
	TestQosReliabilityBasicReliableAck* testQosReliabilityBasicReliableAck_sub_p = &testQosReliabilityBasicReliableAck_sub;

	//TestQosReliabilityBasicBesteffort testQosReliabilityBasicBesteffort_sub;
	//TestQosReliabilityBasicBesteffort* testQosReliabilityBasicBesteffort_sub_p = &testQosReliabilityBasicBesteffort_sub;

	//TestQosReliabilitySmallReliableNack testQosReliabilitySmallReliableNack_sub;
	//TestQosReliabilitySmallReliableNack* testQosReliabilitySmallReliableNack_sub_p = &testQosReliabilitySmallReliableNack_sub;
	//TestQosReliabilityBigReliableNack testQosReliabilityBigReliableNack_sub;
	//TestQosReliabilityBigReliableNack* testQosReliabilityBigReliableNack_sub_p = &testQosReliabilityBigReliableNack_sub;
	//TestQosReliabilityHugeReliableNack testQosReliabilityHugeReliableNack_sub;
	//TestQosReliabilityHugeReliableNack* testQosReliabilityHugeReliableNack_sub_p = &testQosReliabilityHugeReliableNack_sub;


	rc_t retBasic = SDDS_RT_FAIL;
	rc_t retSmall = SDDS_RT_FAIL;
	rc_t retBig = SDDS_RT_FAIL;
	rc_t retHuge = SDDS_RT_FAIL;

	//((Reliable_DataWriter_t*)g_TestQosReliabilityBasicReliableNack_writer)->seqNr = (16-2);
	//((Reliable_DataWriter_t*)g_TestQosReliabilitySmallReliableNack_writer)->seqNr = (256-3);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityBigReliableNack_writer)->seqNr = (65536-4);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityHugeReliableNack_writer)->seqNr = (4294967296-5);

    for (;;) {
		//DDS_TestQosReliabilityBasicReliableNackDataWriter_write (g_TestQosReliabilityBasicReliableNack_writer, &testQosReliabilityBasicReliableNack_pub, NULL);
		//DDS_TestQosReliabilitySmallReliableNackDataWriter_write (g_TestQosReliabilitySmallReliableNack_writer, &testQosReliabilitySmallReliableNack_pub, NULL);
		//DDS_TestQosReliabilityBigReliableNackDataWriter_write (g_TestQosReliabilityBigReliableNack_writer, &testQosReliabilityBigReliableNack_pub, NULL);
		//DDS_TestQosReliabilityHugeReliableNackDataWriter_write (g_TestQosReliabilityHugeReliableNack_writer, &testQosReliabilityHugeReliableNack_pub, NULL);

		//retBasic = DDS_TestQosReliabilityBasicReliableAckDataReader_take_next_sample (g_TestQosReliabilityBasicReliableAck_reader, &testQosReliabilityBasicReliableAck_sub_p, NULL);
		//retSmall = DDS_TestQosReliabilitySmallReliableNackDataReader_take_next_sample (g_TestQosReliabilitySmallReliableNack_reader, &testQosReliabilitySmallReliableNack_sub_p, NULL);
		//retBig = DDS_TestQosReliabilityBigReliableNackDataReader_take_next_sample (g_TestQosReliabilityBigReliableNack_reader, &testQosReliabilityBigReliableNack_sub_p, NULL);
		//retHuge = DDS_TestQosReliabilityHugeReliableNackDataReader_take_next_sample (g_TestQosReliabilityHugeReliableNack_reader, &testQosReliabilityHugeReliableNack_sub_p, NULL);




        sleep (1);
    }

    return 0;
}
