#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "linux_sdds_impl.h"
#include "beta-ds.h"
#include "BuiltinTopic.h"

int main() {
	pthread_t t;
	sDDS_init();
	Log_setLvl(0);

	Discovery_init();

	DDS_ReturnCode_t ret;

	Beta beta_data_used;
	Beta* beta_data_used_ptr = &beta_data_used;

	DDS_DCPSParticipant p_data_used;
	DDS_DCPSParticipant* p_data_used_ptr = &p_data_used;

	DDS_DCPSTopic t_data_used;
	DDS_DCPSTopic* t_data_used_ptr = &t_data_used;

	DDS_DCPSPublication pt_data_used;
	DDS_DCPSPublication* pt_data_used_ptr = &pt_data_used;

	DDS_DCPSSubscription st_data_used;
	DDS_DCPSSubscription* st_data_used_ptr = &st_data_used;

	for (;;) {
		do {
			//static uint8_t foo = 67;
			DDS_DCPSParticipant p;
			p.key = 10;

			DDS_DCPSTopic t;
			memset(t.name, 0, DDS_TOPIC_NAME_SIZE);
			memset(t.type_name, 0, DDS_TOPIC_TYPE_SIZE);
			t.key = 33;
			t.name[0] = 'T';
			t.name[1] = 'e';
			t.name[2] = 'm';
			t.name[3] = 'p';
			t.name[4] = '1';
			t.name[5] = '2';
			t.name[6] = '3';
			t.name[7] = '\0';
			t.type_name[0] = 'A';
			t.type_name[1] = 'A';
			t.type_name[2] = '\0';

			DDS_DCPSPublication pt;
			memset(pt.topic_name, 0, DDS_TOPIC_NAME_SIZE);
			memset(pt.type_name, 0, DDS_TOPIC_TYPE_SIZE);
			pt.key = 55;
			pt.participant_key = 123;
			pt.topic_name[0] = 'T';
			pt.topic_name[1] = 'e';
			pt.topic_name[2] = 'm';
			pt.topic_name[3] = 'p';
			pt.topic_name[4] = '1';
			pt.topic_name[5] = '2';
			pt.topic_name[6] = '3';
			pt.topic_name[7] = '\0';
			pt.type_name[0] = 'B';
			pt.type_name[1] = 'B';
			pt.type_name[2] = '\0';

			DDS_DCPSSubscription st;
			memset(st.topic_name, 0, DDS_TOPIC_NAME_SIZE);
			memset(st.type_name, 0, DDS_TOPIC_TYPE_SIZE);
			st.key = 55;
			st.participant_key = 123;
			st.topic_name[0] = 'T';
			st.topic_name[1] = 'e';
			st.topic_name[2] = 'm';
			st.topic_name[3] = 'p';
			st.topic_name[4] = '1';
			st.topic_name[5] = '2';
			st.topic_name[6] = '3';
			st.topic_name[7] = '\0';
			st.type_name[0] = 'C';
			st.type_name[1] = 'C';
			st.type_name[2] = '\0';

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

			if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer,
					&p, NULL) != DDS_RETCODE_OK) {
				// handle error
				//printf("-----DEBUG-----\t %s:%d\n", __FILE__, __LINE__);
			}

//            if (DDS_DCPSTopicDataWriter_write(g_DCPSTopic_writer, &t, NULL) != DDS_RETCODE_OK) {
//                // handle error
//            	//printf("-----DEBUG-----\t %s:%d\n", __FILE__, __LINE__);
//            }

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

//            if (DDS_DCPSSubscriptionDataWriter_write(g_DCPSSubscription_writer, &st, NULL) != DDS_RETCODE_OK) {
//                // handle error
//            }
//            if (DDS_BetaDataWriter_write(g_Beta_writer, &b, NULL) != DDS_RETCODE_OK) {
//                // handle error
//            }

			do {
				ret = DDS_DCPSParticipantDataReader_take_next_sample(
						g_DCPSParticipant_reader, &p_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data participant\n");
				} else {
					printf("Received (participant):[%u]\n", p_data_used.key);
					ret = Discovery_handleParticipant(p_data_used.key);
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_DCPSTopicDataReader_take_next_sample(
						g_DCPSTopic_reader, &t_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data topic\n");
				} else {
					printf("Received (topic):[%u] topic:%s type:%s\n",
							t_data_used.key, t_data_used.name,
							t_data_used.type_name);
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_DCPSPublicationDataReader_take_next_sample(
						g_DCPSPublication_reader, &pt_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data publication\n");
				} else {
					printf("Received (publication):[%u][%u] topic:%s type:%s\n",
							pt_data_used.key, pt_data_used.participant_key,
							pt_data_used.topic_name, pt_data_used.type_name);
				}
			} while (ret != DDS_RETCODE_NO_DATA);

			do {
				ret = DDS_DCPSSubscriptionDataReader_take_next_sample(
						g_DCPSSubscription_reader, &st_data_used_ptr, NULL);
				if (ret == DDS_RETCODE_NO_DATA) {
					printf("no data subscription\n");
				} else {
					printf(
							"Received (subscription):[%u][%u] topic:%s type:%s\n",
							st_data_used.key, st_data_used.participant_key,
							st_data_used.topic_name, st_data_used.type_name);
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

		} while (0);

	}

}
