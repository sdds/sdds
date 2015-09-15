#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"
#include "DataSource.h"
#include "Discovery.h"
#include "Debug.h"
#include "DataSink.h"

#include "os-ssal/NodeConfig.h"

#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>
#include <sdds/sDDS.h>
#include <sdds/Log.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED

#define SDDS_DISCOVERY_MAX_PARTICIPANTS 50

static SDDS_DCPSParticipant participants[SDDS_DISCOVERY_MAX_PARTICIPANTS];
static pthread_t discoveryThread;

static void *discoveryLoop();
static void Discovery_sendParticipantTopics();
static void Discovery_sendPublicationTopics();
static void Discovery_sendTopicTopics();
static void Discovery_sendSubscriptionTopics(topicid_t topicID);
static void Discovery_receiveParticipantTopics();
static void Discovery_receivePublicationTopics();
static void Discovery_receiveTopicTopics();
static void Discovery_receive_SubscriptionTopics();

static rc_t Discovery_addParticipant(SDDS_DCPSParticipant *p);
static rc_t Discovery_handleParticipant(SDDS_DCPSParticipant p);
static rc_t Discovery_addRemoteDataSink(Locator l, Topic topic);

/************************************************************************************/
/*									Implementierung									*/
/************************************************************************************/

static rc_t Discovery_addParticipant(SDDS_DCPSParticipant *p) {
	int i;
	int freePos = -1;

	if (BuiltinTopic_participantID == p->data.key) {
		return SDDS_RT_KNOWN;
	}

	for (i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		if (participants[i].data.key == p->data.key) {
			return SDDS_RT_KNOWN;
		} else if ((participants[i].data.key == 0) && (freePos == -1)) {
			freePos = i;
		}
	}

	if (freePos != -1) {
		memcpy(&participants[freePos], p, sizeof(SDDS_DCPSParticipant));
		return SDDS_RT_OK;
	}

	return SDDS_RT_FAIL;
}

static rc_t Discovery_handleParticipant(SDDS_DCPSParticipant p) {
	rc_t ret = Discovery_addParticipant(&p);
	if (ret == SDDS_RT_OK) {
#if defined(SDDS_TOPIC_HAS_PUB)
		ret = Discovery_addRemoteDataSink(p.addr, g_DCPSSubscription_topic);
#endif

#if defined(SDDS_TOPIC_HAS_SUB)
		ret = Discovery_addRemoteDataSink(p.addr, g_DCPSPublication_topic);
		Discovery_sendPublicationTopics();
#endif

		if (ret != SDDS_RT_OK) {
			return ret;
		}

	}

	Locator_downRef(p.addr);

	return ret;
}

static rc_t Discovery_addRemoteDataSink(Locator l, Topic topic) {
	rc_t ret;

	ret = Topic_addRemoteDataSink(topic, l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_downRef(l);

	return ret;
}

static void Discovery_sendParticipantTopics() {
	DDS_DCPSParticipant p;
	p.key = BuiltinTopic_participantID;

	if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer, &p,
	NULL) != DDS_RETCODE_OK) {
		// handle error
		Log_error("Send participant topic failed.\n");
	}
}

static void Discovery_sendPublicationTopics() {
#if defined(SDDS_TOPIC_HAS_SUB)
	rc_t ret;
	DDS_DCPSPublication pubT[20];
	int len = 0;

	ret = DataSource_getDataWrites(pubT, &len);

	int i;
	for (i = 0; i < len; i++) {
		if (DDS_DCPSPublicationDataWriter_write(g_DCPSPublication_writer,
				&pubT[i],
				NULL) != DDS_RETCODE_OK) {
			// handle error
			Log_error("Send publication topic failed.\n");
		}
	}
#endif
}

void Discovery_sendTopicTopics() {
	// TO DO
}

static void Discovery_sendSubscriptionTopics(topicid_t topicID) {
	SDDS_DCPSSubscription st_data_used;
	DDS_ReturnCode_t ret;

	ret = DataSink_getTopic((DDS_DCPSSubscription *) &st_data_used,
			topicID, NULL);
	if (ret == SDDS_RT_OK) {
		if (DDS_DCPSSubscriptionDataWriter_write(g_DCPSSubscription_writer,
				(DDS_DCPSSubscription *) &st_data_used,
				NULL) != DDS_RETCODE_OK) {
			// handle error
			Log_error("Send subscription topic failed.\n");
		}
	}
}

static void Discovery_receiveParticipantTopics() {
	DDS_ReturnCode_t ret;
	SDDS_DCPSParticipant p_data_used;
	SDDS_DCPSParticipant* p_data_used_ptr = &p_data_used;

	do {
		ret = DDS_DCPSParticipantDataReader_take_next_sample(
				g_DCPSParticipant_reader, (DDS_DCPSParticipant **) &p_data_used_ptr, NULL);
		if (ret == DDS_RETCODE_NO_DATA) {
#ifdef UTILS_DEBUG
			Log_debug("no participant data\n");
#endif
		} else {
			Log_debug("Received (participant):[%x]\n", p_data_used.data.key);
			ret = Discovery_handleParticipant(p_data_used);
		}
	} while (ret != DDS_RETCODE_NO_DATA);
}

static void Discovery_receivePublicationTopics() {
#if defined(SDDS_TOPIC_HAS_PUB)
	rc_t ret;
	DDS_DCPSPublication pt_data_used;
	DDS_DCPSPublication* pt_data_used_ptr = &pt_data_used;

	do {
		ret = DDS_DCPSPublicationDataReader_take_next_sample(
				g_DCPSPublication_reader, &pt_data_used_ptr, NULL);
		if (ret == DDS_RETCODE_NO_DATA) {
#ifdef UTILS_DEBUG
			Log_debug("no publication data\n");
#endif
		} else {
			Log_debug("Received (publication):[%u][%x] topic:%u\n",
					pt_data_used.key, pt_data_used.participant_key,
					pt_data_used.topic_id);
			Discovery_sendSubscriptionTopics(pt_data_used.topic_id);
		}
	}while (ret != DDS_RETCODE_NO_DATA);
#endif
}

static void Discovery_receiveTopicTopics() {
	// TO DO
}

static void Discovery_receive_SubscriptionTopics() {
#if defined(SDDS_TOPIC_HAS_SUB)
	SDDS_DCPSSubscription st_data_used;
	SDDS_DCPSSubscription* st_data_used_ptr = &st_data_used;
	rc_t ret;

	do {
		ret = DDS_DCPSSubscriptionDataReader_take_next_sample(
				g_DCPSSubscription_reader, (DDS_DCPSSubscription **) &st_data_used_ptr, NULL);
		if (ret == DDS_RETCODE_NO_DATA) {
#ifdef UTILS_DEBUG
			Log_debug("no subscription data\n");
#endif
		} else {
			Log_debug("Received (subscription):[%u][%x] topic:%u\n",
					st_data_used.data.key, st_data_used.data.participant_key,
					st_data_used.data.topic_id);
			Topic topic = NULL;
			ret = DataSource_getTopic(NULL, st_data_used.data.topic_id, &topic);
			if (ret == SDDS_RT_OK) {
				ret = Discovery_addRemoteDataSink(st_data_used.addr, topic);
			}
		}
	} while (ret != DDS_RETCODE_NO_DATA);
#endif
}

static void *discoveryLoop() {
	int publicationTime = 0;

	while (true) {
		publicationTime += SDDS_DISCOVERY_PARTICIPANT_TIMER;

		Discovery_sendParticipantTopics();

		if (publicationTime >= SDDS_DISCOVERY_PUBLICATION_TIMER) {
			Discovery_sendPublicationTopics();
			publicationTime = 0;
		}

		Discovery_sendTopicTopics();

		Discovery_receiveParticipantTopics();

		Discovery_receivePublicationTopics();

		Discovery_receiveTopicTopics();

		Discovery_receive_SubscriptionTopics();

		sleep(SDDS_DISCOVERY_PARTICIPANT_TIMER);
	}
}

rc_t Discovery_init() {
	for (int i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		participants[i].data.key = 0;
	}

	// set up a thread to read from the udp socket
	if (pthread_create(&discoveryThread, NULL, discoveryLoop, NULL) != 0) {
		exit(-1);
	}

	return SDDS_RT_OK;
}

#endif
