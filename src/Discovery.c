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
#include "os-ssal/Thread.h"
#include "os-ssal/Memory.h"
#include "os-ssal/Task.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>
#include <sdds/sDDS.h>
#include <sdds/Log.h>

#include <dds/DDS_DCPS.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED

#ifndef SDDS_DISCOVERY_MAX_PARTICIPANTS
#define SDDS_DISCOVERY_MAX_PARTICIPANTS 10
#endif

#define SDDS_DISCOVERY_SLEEP_TIME		1

static SDDS_DCPSParticipant participants[SDDS_DISCOVERY_MAX_PARTICIPANTS];
static Task recvTask;
static Task sendPartTask;
static Task sendPubTask;
static Task sendTopTask;

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
static rc_t Discovery_addRemoteDataSink(Locator_t* l, Topic_t *topic);

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

static rc_t Discovery_addRemoteDataSink(Locator_t* l, Topic_t *topic) {
	rc_t ret;

	Locator_t* loc;
	ret = Network_createMulticastLocator(&loc);
	ret = Network_setMulticastAddressToLocator(loc, SDDS_BUILTIN_SUB_PUB_ADDRESS);

	if (!Locator_isEqual(l, loc)) {
		ret = Topic_addRemoteDataSink(topic, l);
		if (ret != SDDS_RT_OK) {
			return ret;
		}
	}

	Locator_downRef(l);

	return ret;
}

static void Discovery_sendParticipantTopics(void *data) {
	DDS_DCPSParticipant p;
	p.key = BuiltinTopic_participantID;

	if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer, &p,
	NULL) == DDS_RETCODE_ERROR) {
		// handle error
		Log_error("Send participant topic failed.\n");
	}
}

static void Discovery_sendPublicationTopics(void *data) {
#if defined(SDDS_TOPIC_HAS_SUB)
	rc_t ret;
	DDS_DCPSPublication pubT[20];
	int len = 0;

	ret = DataSource_getDataWrites(pubT, &len);

	if (ret == SDDS_RT_OK) {
		int i;
		for (i = 0; i < len; i++) {
			if (DDS_DCPSPublicationDataWriter_write(g_DCPSPublication_writer,
					&pubT[i],
					NULL) == DDS_RETCODE_ERROR) {
				// handle error
				Log_error("Send publication topic failed.\n");
			}
		}
	}
#endif
}

void Discovery_sendTopicTopics(void *data) {
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
				NULL) == DDS_RETCODE_ERROR) {
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

	topicid_t tIDs[SDDS_DISCOVERY_MAX_PARTICIPANTS];
	memset(tIDs, 0, SDDS_DISCOVERY_MAX_PARTICIPANTS);
	bool sub = true;
	int nextID = 0;

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
			for (int i = 0; i <= nextID; i++) {
				if (tIDs[i] == pt_data_used.topic_id) {
					sub = false;
					break;
				}
			}
			if (sub) {
				tIDs[nextID] = pt_data_used.topic_id;
				nextID = (nextID + 1) % SDDS_DISCOVERY_MAX_PARTICIPANTS;
				Discovery_sendSubscriptionTopics(pt_data_used.topic_id);
			}
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
			Topic_t *topic = NULL;
			ret = DataSource_getTopic(NULL, st_data_used.data.topic_id, &topic);
			if (ret == SDDS_RT_OK) {
				ret = Discovery_addRemoteDataSink(st_data_used.addr, topic);
			}
		}
	} while (ret != DDS_RETCODE_NO_DATA);
#endif
}

void Discovery_receive(void *data) {
	Discovery_receiveParticipantTopics();

	Discovery_receivePublicationTopics();

	Discovery_receiveTopicTopics();

	Discovery_receive_SubscriptionTopics();
}

rc_t Discovery_init() {
	for (int i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		participants[i].data.key = 0;
	}

	TaskMng_init();

#if (SDDS_DISCOVERY_RECEIVE_TIMER != 0)
	recvTask = Task_create();
	Task_init(recvTask, Discovery_receive, NULL);
	if (Task_start(recvTask, SDDS_DISCOVERY_RECEIVE_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
		Log_error("Task_start failed\n");
	}
#endif

#if (SDDS_DISCOVERY_PARTICIPANT_TIMER != 0)
	sendPartTask = Task_create();
	Task_init(sendPartTask, Discovery_sendParticipantTopics, NULL);
	if (Task_start(sendPartTask, SDDS_DISCOVERY_PARTICIPANT_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
		Log_error("Task_start failed\n");
	}
#endif

#if (SDDS_DISCOVERY_PUBLICATION_TIMER != 0)
	sendPubTask = Task_create();
	Task_init(sendPubTask, Discovery_sendPublicationTopics, NULL);
	if (Task_start(sendPubTask, SDDS_DISCOVERY_PUBLICATION_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
		Log_error("Task_start failed\n");
	}
#endif

#if (SDDS_DISCOVERY_TOPIC_TIMER != 0)
	sendTopTask = Task_create();
	Task_init(sendTopTask, Discovery_sendTopicTopics, NULL);
	if (Task_start(sendTopTask, SDDS_DISCOVERY_TOPIC_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
		Log_error("Task_start failed\n");
	}
#endif

	return SDDS_RT_OK;
}

#endif
