#include <unistd.h>
#include "linux_steffen1_sdds_impl.h"
#include "unistd.h"
#include <pthread.h>


uint32_t g_seqNrToSet = 0;

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
		scanf("%d", &g_seqNrToSet);
		writer_basic_p->seqNr = g_seqNrToSet;
		printf("INFO: Set seqNr to %d\n", g_seqNrToSet);
	}
}

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(5);

	writer_basic_p = (Reliable_DataWriter_t*)g_TestQosReliabilityBasicReliableAck_writer;

	pthread_t p1;

	pthread_create (&p1, NULL, getKeyboardInput, NULL);

	//TestQosReliabilityBasicBesteffort testQosReliabilityBasicBesteffort_pub;
	TestQosReliabilityBasicReliableAck testQosReliabilityBasicReliableAck_pub;
	//TestQosReliabilitySmallReliableNack testQosReliabilitySmallReliableNack_pub;
	//TestQosReliabilityBigReliableNack testQosReliabilityBigReliableNack_pub;
	//TestQosReliabilityHugeReliableNack testQosReliabilityHugeReliableNack_pub;

	//TestQosReliabilityBasicReliableNack testQosReliabilityBasicReliableNack_sub;
	//TestQosReliabilityBasicReliableNack* testQosReliabilityBasicReliableNack_sub_p = &testQosReliabilityBasicReliableNack_sub;
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

	writer_basic_p = (Reliable_DataWriter_t*)g_TestQosReliabilityBasicReliableAck_writer;

	//((Reliable_DataWriter_t*)g_TestQosReliabilityBasicReliableNack_writer)->seqNr = (16-2);
	//((Reliable_DataWriter_t*)g_TestQosReliabilitySmallReliableNack_writer)->seqNr = (256-3);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityBigReliableNack_writer)->seqNr = (65536-4);
	//((Reliable_DataWriter_t*)g_TestQosReliabilityHugeReliableNack_writer)->seqNr = (4294967296-5);

    for (;;) {
		//DDS_TestQosReliabilityBasicReliableAckDataWriter_write (g_TestQosReliabilityBasicReliableAck_writer, &testQosReliabilityBasicReliableAck_pub, NULL);
		DDS_TestQosReliabilityBasicReliableAckDataWriter_write (g_TestQosReliabilityBasicReliableAck_writer, &testQosReliabilityBasicReliableAck_pub, NULL);
		//printf("seqNr:%d\n", writer_basic_p->seqNr);


		printf("\n");
		for (int i=0; i<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; i++){
			printf("slot: %d isUsed: %d seqNr: %d\n", i, writer_basic_p->samplesToKeep[i].isUsed, writer_basic_p->samplesToKeep[i].seqNr);
		}

		//DDS_TestQosReliabilityBasicReliableNackDataWriter_write (g_TestQosReliabilityBasicReliableNack_writer, &testQosReliabilityBasicReliableNack_pub, NULL);
		//DDS_TestQosReliabilitySmallReliableNackDataWriter_write (g_TestQosReliabilitySmallReliableNack_writer, &testQosReliabilitySmallReliableNack_pub, NULL);
		//DDS_TestQosReliabilityBigReliableNackDataWriter_write (g_TestQosReliabilityBigReliableNack_writer, &testQosReliabilityBigReliableNack_pub, NULL);
		//DDS_TestQosReliabilityHugeReliableNackDataWriter_write (g_TestQosReliabilityHugeReliableNack_writer, &testQosReliabilityHugeReliableNack_pub, NULL);

		//retBasic = DDS_TestQosReliabilityBasicReliableNackDataReader_take_next_sample (g_TestQosReliabilityBasicReliableNack_reader, &testQosReliabilityBasicReliableNack_sub_p, NULL);
		//retSmall = DDS_TestQosReliabilitySmallReliableNackDataReader_take_next_sample (g_TestQosReliabilitySmallReliableNack_reader, &testQosReliabilitySmallReliableNack_sub_p, NULL);
		//retBig = DDS_TestQosReliabilityBigReliableNackDataReader_take_next_sample (g_TestQosReliabilityBigReliableNack_reader, &testQosReliabilityBigReliableNack_sub_p, NULL);
		//retHuge = DDS_TestQosReliabilityHugeReliableNackDataReader_take_next_sample (g_TestQosReliabilityHugeReliableNack_reader, &testQosReliabilityHugeReliableNack_sub_p, NULL);


        sleep (1);
    }

    return 0;
}
