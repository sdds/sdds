#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "linux_sdds_impl.h"
#include "beta-ds.h"
#include "BuiltinTopic.h"
#include "Discovery.h"

#include "os-ssal/NodeConfig.h"

void printRC(rc_t ret);

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

        DDS_DCPSParticipant p;
        p.key = BuiltinTopic_participantID;

		if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer, &p, NULL) != DDS_RETCODE_OK) {
			// handle error
		}

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

	}
}
