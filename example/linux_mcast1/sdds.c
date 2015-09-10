#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "linux_sdds_impl.h"
#include "beta-ds.h"
#include "BuiltinTopic.h"
#include "Log.h"
#include "DataSource.h"
#include "DataSink.h"
#include "Discovery.h"

#include "os-ssal/NodeConfig.h"

int main() {
	//pthread_t td;
	sDDS_init();
	Log_setLvl(0);

	Discovery_init();

	DDS_ReturnCode_t ret;

	Beta beta_data_used;
	Beta* beta_data_used_ptr = &beta_data_used;

	SDDS_DCPSParticipant p_data_used;
	SDDS_DCPSParticipant* p_data_used_ptr = &p_data_used;

	DDS_DCPSTopic t_data_used;
	DDS_DCPSTopic* t_data_used_ptr = &t_data_used;

	DDS_DCPSPublication pt_data_used;
	DDS_DCPSPublication* pt_data_used_ptr = &pt_data_used;

	SDDS_DCPSSubscription st_data_used;
	SDDS_DCPSSubscription* st_data_used_ptr = &st_data_used;

	DDS_DCPSParticipant p;
	p.key = BuiltinTopic_participantID;

	DDS_DCPSTopic t;
	memset(t.name, 0, DDS_TOPIC_NAME_SIZE);
	t.key = 33;
	t.name[0] = 'T';
	t.name[1] = 'e';
	t.name[2] = 'm';
	t.name[3] = 'p';
	t.name[4] = '1';
	t.name[5] = '2';
	t.name[6] = '3';
	t.name[7] = '\0';

	Beta b;
	b.value = 44;
	memset(b.value2, 0, 11);
	memset(b.value3, 0, 10);
	b.value2[0] = 'D';
	b.value2[1] = 'e';
	b.value2[2] = 'r';
	b.value2[3] = '\0';
	b.value3[0] = 'B';
	b.value3[1] = 'a';
	b.value3[2] = 'u';
	b.value3[3] = 'm';
	b.value3[4] = '!';
	b.value3[5] = '\0';

	int b_val = 0;
	int pCount = 0;

//	char test[2];
//	printf("test = %c\n", test[2]);

	for (;;) {
			if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer,
					&p, NULL) != DDS_RETCODE_OK) {
				// handle error
				//printf("-----DEBUG-----\t %s:%d\n", __FILE__, __LINE__);
			}

			DDS_DCPSPublication pubT[20];
			int len = 0;
			ret = DaraSource_getDataWrites(pubT, &len);

			int i;
			for (i = 0; i < len; i++) {
				if (DDS_DCPSPublicationDataWriter_write(
						g_DCPSPublication_writer, &pubT[i],
						NULL) != DDS_RETCODE_OK) {
					// handle error
				}
			}


			b.value = b_val++;
			if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
					NULL) != DDS_RETCODE_OK) {
				// handle error
			}

			do {
				ret = DDS_DCPSParticipantDataReader_take_next_sample(
						g_DCPSParticipant_reader, &p_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data participant\n");
				} else {
					printf("Received (participant):[%x]\n", p_data_used.data.key);
					ret = Discovery_handleParticipant(p_data_used);
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_DCPSPublicationDataReader_take_next_sample(
						g_DCPSPublication_reader, &pt_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data publication\n");
				} else {
					printf("Received (publication):[%u][%x] topic:%u\n",
							pt_data_used.key, pt_data_used.participant_key,
							pt_data_used.topic_id);

					ret = DataSink_getTopic((struct DDS_DCPSSubscription *) &st_data_used, pt_data_used.topic_id, NULL);
					if (ret == SDDS_RT_OK) {
						if (DDS_DCPSSubscriptionDataWriter_write(g_DCPSSubscription_writer, (struct DDS_DCPSSubscription *) &st_data_used, 	NULL) != DDS_RETCODE_OK) {
							// handle error
						}
					}
				}
			} while (ret != DDS_RETCODE_NO_DATA);

//			do {
//				ret = DDS_DCPSTopicDataReader_take_next_sample(
//						g_DCPSTopic_reader, &t_data_used_ptr, NULL);
//				if (ret == DDS_RETCODE_NO_DATA) {
//					printf("no data topic\n");
//				} else {
//					printf("Received (topic):[%x] topic:%s\n", t_data_used.key,
//							t_data_used.name);
//				}
//			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_DCPSSubscriptionDataReader_take_next_sample(
						g_DCPSSubscription_reader, &st_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data subscription\n");
				} else {
					printf("Received (subscription):[%u][%x] topic:%u\n",
							st_data_used.data.key, st_data_used.data.participant_key,
							st_data_used.data.topic_id);

					Topic topic = NULL;
					ret = DataSink_getTopic(NULL, st_data_used.data.topic_id, &topic);
					if (ret == SDDS_RT_OK) {
						ret = Discovery_addRemoteDataSinkLoc(st_data_used.addr, topic);
					}
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader,
						&beta_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data beta\n");
				} else {
					printf("Received (beta):[%d] %s %s\n", beta_data_used.value,
							beta_data_used.value2, beta_data_used.value3);
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			sleep(10);
	}

}
