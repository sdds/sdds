/*
 * =====================================================================================
 *
 *       Filename:  DataSource.c
 *
 *    Description:  Impl for the datasource class
 *
 *        Version:  1.0
 *        Created:  22.02.2010 18:09:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "DataSource.h"
#include "sDDS.h"
#include "Log.h"
#include "Qos.h"
#include "SNPS.h"
#include "NetBuffRef.h"
#include "Network.h"
#include "Marshalling.h"
#include "os-ssal/Task.h"

#ifndef SDDS_PLATFORM_autobest
#include <stdlib.h>
#endif
#include <string.h>

// IF BUILTINTOPIC
#include "BuiltinTopic.h"

struct _InstantSender_t {
	NetBuffRef_t highPrio;
	NetBuffRef_t out[SDDS_NET_MAX_OUT_QUEUE];
};
typedef struct _InstantSender_t InstantSender_t;

struct _DataWriter_t {
	Topic_t *topic;
	struct SourceQos_t qos;
	unsigned int id :4;
};

struct _DataSource_t {
#if SDDS_MAX_DATA_WRITERS > 0
	DataWriter_t writers[SDDS_MAX_DATA_WRITERS];
#endif
	InstantSender_t sender;

#if defined(__GNUC__) && __GNUC_MINOR__ < 6
#pragma GCC diagnostic error "-Woverflow"
#endif
	unsigned int remaining_datawriter :4;
};

static DataSource_t dsStruct;
static Task sendTask;

DataSource_t *dataSource = &dsStruct;

NetBuffRef_t * findFreeFrame(Locator_t* dest);
rc_t checkSending(NetBuffRef_t *buf);
void checkSendingWrapper(void *buf);

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t DataSource_getTopic(DDS_DCPSSubscription *st, topicid_t id, Topic_t **topic) {
	int i;
	for (i = 0; i < (SDDS_MAX_DATA_WRITERS - dataSource->remaining_datawriter);
			i++) {
		if ((dataSource->writers[i].topic->id == id)) {
			if (st != NULL) {
				st->key = dataSource->writers[i].id;
				st->participant_key = BuiltinTopic_participantID;
				st->topic_id = dataSource->writers[i].topic->id;
			}
			if (topic != NULL) {
				*topic = dataSource->writers[i].topic;
			}
			return SDDS_RT_OK;
		}
	}

	return SDDS_RT_FAIL;
}
#endif

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t DataSource_getDataWrites(DDS_DCPSPublication *pt, int *len) {
	int i = 0;
	*len = 0;

	for (i = 0; i < (SDDS_MAX_DATA_WRITERS - dataSource->remaining_datawriter);
			i++) {
	#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
		if (!BuildinTopic_isBuiltinTopic(dataSource->writers[i].topic->id,
						dataSource->writers[i].topic->domain)) {
	#endif
			pt[*len].key = dataSource->writers[i].id;
			pt[*len].participant_key = BuiltinTopic_participantID;
			pt[*len].topic_id = dataSource->writers[i].topic->id;

			(*len)++;
	#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
		}
	#endif
	}

	return SDDS_RT_OK;
}
#endif

rc_t DataSource_init(void) {
	if (TimeMng_init() != SDDS_RT_OK) {
		return SDDS_RT_FAIL;
	}
	sendTask = Task_create();
	ssw_rc_t ret = Task_init(sendTask, checkSendingWrapper, NULL);
	if (ret == SDDS_SSW_RT_FAIL) {
		Log_error("Task_init failed\n");
		return SDDS_RT_FAIL;
	}

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Woverflow"
#endif

	dataSource->remaining_datawriter = SDDS_MAX_DATA_WRITERS;

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#endif

	// init instant sender
	// the init method alloc a frame buffer and adds it to the structure
	NetBuffRef_init(&(dataSource->sender.highPrio));
	NetBuffRef_init(&(dataSource->sender.out[0]));
	NetBuffRef_init(&(dataSource->sender.out[1]));

	return SDDS_RT_OK;
}

#if SDDS_MAX_DATA_WRITERS > 0
DataWriter_t * DataSource_create_datawriter(Topic_t *topic, Qos qos,
		Listener list, StatusMask mask) {
	(void) qos;
	(void) list;
	(void) mask;

	DataWriter_t *dw = NULL;

	if (dataSource->remaining_datawriter == 0) {
		return NULL;
	}
	dw = &(dataSource->writers[SDDS_MAX_DATA_WRITERS
			- dataSource->remaining_datawriter]);

	dw->topic = topic;
	dw->id = (SDDS_MAX_DATA_WRITERS - dataSource->remaining_datawriter);
	dataSource->remaining_datawriter--;

	#ifdef SDDS_QOS_LATENCYBUDGET
	dw->qos.latBudDuration = SDDS_QOS_DW_LATBUD;
	#endif
	return dw;
}
#endif // SDDS_MAX_DATA_WRITERS

NetBuffRef_t *findFreeFrame(Locator_t* dest) {
	NetBuffRef_t *buffRef = NULL;

	bool_t sameAddr = false;
	for (int i = 0; i < SDDS_NET_MAX_OUT_QUEUE; i++) {
		Locator_t* try = dataSource->sender.out[i].addr;
		if (dest != NULL && try != NULL && Locator_isEqual(dest, try)) {
			buffRef = &(dataSource->sender.out[i]);
			sameAddr = true;
			break;
		}
	}
	if (buffRef == NULL) {
		for (int i = 0; i < SDDS_NET_MAX_OUT_QUEUE; i++) {
			if (dataSource->sender.out[i].curPos == 0) {
				buffRef = &(dataSource->sender.out[i]);
				break;
			}
		}
	}
	if (buffRef == NULL) {
		buffRef = &(dataSource->sender.highPrio);
	}
	if (buffRef->curPos == 0) {
		SNPS_initFrame(buffRef);
	}
	if (sameAddr == false) {
		// write addr

		// here add the ref to the buff, addr is used when frame is update addr
		// in bufref
		buffRef->addr = dest;
	}
	return buffRef;
}

void checkSendingWrapper(void *buf) {
	Log_debug("callback\n");
	checkSending((NetBuffRef_t*) buf);
}

rc_t checkSending(NetBuffRef_t *buf) {
#ifdef SDDS_QOS_LATENCYBUDGET
	#if SDDS_QOS_DW_LATBUD < 65536
	time16_t time;
	Time_getTime16(&time);
	#else
	time32_t time;
	Time_getTime32(&time);
	#endif

	if ((buf->sendDeadline <= time) || (SDDS_NET_MAX_BUF_SIZE <= buf->curPos)) {
		Task_stop(sendTask);
		if (buf->sendDeadline >= time) {
			Log_debug("OK: deadline: %d >= time: %d\n", buf->sendDeadline, time);
		}
		else if ((buf->sendDeadline < time) && (buf->addr == NULL)) {
			Log_debug("OK: deadline: %d < time: %d --> No dst address\n", buf->sendDeadline, time);
		}
		else {
			Log_debug("ERROR: deadline: %d < time: %d\n", buf->sendDeadline, time);
		}
#endif
		// update header
		SNPS_updateHeader(buf);

		if (buf->addr != NULL) {

			if (Network_send(buf) != SDDS_RT_OK) {
				return SDDS_RT_FAIL;
			}
			// is frame is send free the buffer
			NetBuffRef_renew(buf);
		}
		else {
			buf->sendDeadline = 0;
		}

		return SDDS_RT_OK;
	}
#ifdef SDDS_QOS_LATENCYBUDGET
	else {
		Task_stop(sendTask);
		Task_setData(sendTask, (void*) buf);
#if SDDS_QOS_DW_LATBUD < 65536
		msecu16_t taskTime;
		uint16_t taskSec;
		msecu16_t taskMSec;
#else
		msecu32_t taskTime;
		uint32_t taskSec;
		msecu32_t taskMSec;
#endif
		taskTime = (buf->sendDeadline - time);
		taskSec = taskTime / 1000;
		taskMSec = taskTime % 1000;
		ssw_rc_t ret = Task_start(sendTask, taskSec, taskMSec, SDDS_SSW_TaskMode_single);
		if (ret != SDDS_RT_OK) {
			Log_error("Task_start failed\n");
			return SDDS_RT_FAIL;
		}

#ifdef UTILS_DEBUG
		Log_debug("Test startet, timer: %d\n", (buf->sendDeadline - time));
		Log_debug("%d > %d\n", buf->sendDeadline, time);
#endif
		return SDDS_RT_DEFERRED;
	}
#endif
}

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t DataSource_write(DataWriter_t *_this, Data data, void* waste) {

	(void) waste;
	NetBuffRef_t *buffRef = NULL;
	Topic_t *topic = _this->topic;
	domainid_t domain = topic->domain;
	Locator_t* dest = topic->dsinks.list;
	rc_t ret;

#ifdef SDDS_QOS_LATENCYBUDGET
	#if SDDS_QOS_DW_LATBUD < 65536
	msecu16_t latBudDuration = _this->qos.latBudDuration;
	time16_t deadline;
	ret = Time_getTime16(&deadline);
	#else
	msecu32_t latBudDuration = _this->qos.latBudDuration;
	time32_t deadline;
	rc_t ret = Time_getTime32(&deadline);
	#endif

	#ifdef UTILS_DEBUG
	Log_debug("dw ID: %d, latBud: %d time: %d\n", _this->id, _this->qos.latBudDuration, deadline);
	#endif

	// to do exact calculation
	deadline += (latBudDuration - SDDS_QOS_LATBUD_COMM - SDDS_QOS_LATBUD_READ);
#endif
	buffRef = findFreeFrame(dest);
	buffRef->addr = dest;
#ifdef SDDS_QOS_LATENCYBUDGET
	//  If new deadline is earlier
	if ((buffRef->sendDeadline == 0)) {
		buffRef->sendDeadline = deadline;
		Log_debug("sendDeadline: %d\n", buffRef->sendDeadline);
	}
#endif

	if (buffRef->curDomain != domain) {
		SNPS_writeDomain(buffRef, domain);
		buffRef->curDomain = domain;
	}

	if (buffRef->curTopic != topic) {
		SNPS_writeTopic(buffRef, topic->id);
		buffRef->curTopic = topic;
	}

	if (SNPS_writeData(buffRef, topic->Data_encode, data) != SDDS_RT_OK) {
		// something went wrong oO
		return SDDS_RT_FAIL;
	}

#ifdef SDDS_QOS_RELIABILITY
    #if SDDS_QOS_RELIABILITY_KIND == KIND_BESTEFFORT
        #if SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_NORMAL
	    if (SNPS_writeSeqNr(buffRef, _this->qos.seqNr) != SDDS_RT_OK) {
            return SDDS_RT_FAIL;
        }
        #elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
	    if (SNPS_writeSeqNrSmall(buffRef, _this->qos.seqNr) != SDDS_RT_OK) {
            return SDDS_RT_FAIL;
        }
        #elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_BIG
	    if (SNPS_writeSeqNrBig(buffRef, _this->qos.seqNr) != SDDS_RT_OK) {
            return SDDS_RT_FAIL;
        }
        #elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
	    if (SNPS_writeSeqNrHUGE(buffRef, _this->qos.seqNr) != SDDS_RT_OK) {
            return SDDS_RT_FAIL;
        }
        #endif
    _this->qos.seqNr++;

    #else
        //TODO
    #endif
#endif

	Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);
	// return 0;

	ret = checkSending(buffRef);
	if (ret == SDDS_RT_FAIL) {
		return ret;
	}
	else {
		return SDDS_RT_OK;
	}
}
#endif // SDDS_TOPIC_HAS_SUB

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
rc_t DataSource_writeAddress(DataWriter_t *_this, castType_t castType, addrType_t addrType, uint8_t *addr, uint8_t addrLen) {
	NetBuffRef_t *buffRef = NULL;
	Topic_t *topic = _this->topic;
	domainid_t domain = topic->domain;
	Locator_t* dest = topic->dsinks.list;

	buffRef = findFreeFrame(dest);
	buffRef->addr = dest;

	if (buffRef->curDomain != domain) {
		SNPS_writeDomain(buffRef, domain);
		buffRef->curDomain = domain;
	}
	if (buffRef->curTopic != topic) {
		SNPS_writeTopic(buffRef, topic->id);
		buffRef->curTopic = topic;
	}

	if (SNPS_writeAddress(buffRef, castType, addrType, addr, addrLen) != SDDS_RT_OK) {
		// something went wrong oO
		return SDDS_RT_FAIL;
	}

	Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);

	return SDDS_RT_OK;
}
#endif
