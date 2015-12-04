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

#include "sDDS.h"

#ifndef SDDS_PLATFORM_autobest
#include <stdlib.h>
#endif
#include <string.h>

struct _InstantSender_t {
    NetBuffRef_t highPrio;
    NetBuffRef_t out[SDDS_NET_MAX_OUT_QUEUE];
};
typedef struct _InstantSender_t InstantSender_t;

struct _DataSource_t {
#if SDDS_MAX_DATA_WRITERS > 0
    DataWriter_t writers[SDDS_MAX_DATA_WRITERS];
#endif
    InstantSender_t sender;
    unsigned int remaining_datawriter : 4;
};

static DataSource_t dsStruct;
static Task sendTask;

static DataSource_t* self = &dsStruct;

//  Forward declarations of internal helper functions

rc_t
checkSending(NetBuffRef_t* buf);
void
checkSendingWrapper(void* buf);

rc_t
DataSource_init(void) {
    if (TimeMng_init() != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    sendTask = Task_create();
    ssw_rc_t ret = Task_init(sendTask, checkSendingWrapper, NULL);
    if (ret == SDDS_SSW_RT_FAIL) {
        Log_error("Task_init failed\n");
        return SDDS_RT_FAIL;
    }
    self->remaining_datawriter = SDDS_MAX_DATA_WRITERS;

    //  Init instant sender
    //  The init method alloc a frame buffer and adds it to the structure
    NetBuffRef_init(&(self->sender.highPrio));
    NetBuffRef_init(&(self->sender.out[0]));
    NetBuffRef_init(&(self->sender.out[1]));

    return SDDS_RT_OK;
}


#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t
DataSource_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic) {
    int i;
    for (i = 0; i < (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter);
         i++) {
        if ((self->writers[i].topic->id == id)) {
            if (st != NULL) {
                st->key = self->writers[i].id;
                st->participant_key = BuiltinTopic_participantID;
                st->topic_id = self->writers[i].topic->id;
            }
            if (topic != NULL) {
                *topic = self->writers[i].topic;
            }
            return SDDS_RT_OK;
        }
    }

    return SDDS_RT_FAIL;
}
#endif

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t
DataSource_getDataWrites(DDS_DCPSPublication* pt, int* len) {
    int i = 0;
    *len = 0;

    for (i = 0; i < (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter); i++) {
#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
        if (!BuildinTopic_isBuiltinTopic(self->writers[i].topic->id,
                                         self->writers[i].topic->domain)) {
#endif
        pt[*len].key = self->writers[i].id;
        pt[*len].participant_key = BuiltinTopic_participantID;
        pt[*len].topic_id = self->writers[i].topic->id;

        (*len)++;
#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
    }
#endif
    }
    return SDDS_RT_OK;
}
#endif

#if SDDS_MAX_DATA_WRITERS > 0
DataWriter_t*
DataSource_create_datawriter(Topic_t* topic, Qos qos,
                             Listener list, StatusMask mask) {
    (void) qos;
    (void) list;
    (void) mask;

    DataWriter_t* dw = NULL;

    if (self->remaining_datawriter == 0) {
        return NULL;
    }
    dw = &(self->writers[SDDS_MAX_DATA_WRITERS - self->remaining_datawriter]);

    dw->topic = topic;
    dw->id = (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter);
    self->remaining_datawriter--;

#ifdef SDDS_QOS_LATENCYBUDGET
    dw->qos.latBudDuration = SDDS_QOS_DW_LATBUD;
#endif
    return dw;
}
#endif // SDDS_MAX_DATA_WRITERS

NetBuffRef_t*
findFreeFrame(Locator_t* dest) {
    NetBuffRef_t* buffRef = NULL;

    bool_t sameAddr = false;
    for (int i = 0; i < SDDS_NET_MAX_OUT_QUEUE; i++) {
        Locator_t* try = self->sender.out[i].addr;
        if (dest != NULL && try != NULL && Locator_isEqual(dest, try)) {
            buffRef = &(self->sender.out[i]);
            sameAddr = true;
            break;
        }
    }
    if (buffRef == NULL) {
        for (int i = 0; i < SDDS_NET_MAX_OUT_QUEUE; i++) {
            if (self->sender.out[i].curPos == 0) {
                buffRef = &(self->sender.out[i]);
                break;
            }
        }
    }
    if (buffRef == NULL) {
        buffRef = &(self->sender.highPrio);
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

void
checkSendingWrapper(void* buf) {
    Log_debug("callback\n");
    checkSending((NetBuffRef_t*) buf);
}

rc_t
checkSending(NetBuffRef_t* buf) {
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
        //  If frame was sent, free the buffer.
        NetBuffRef_renew(buf);
    }
    //  If latencyBudget is active, don't discard the buffer right away.
#ifdef SDDS_QOS_LATENCYBUDGET
    //  Discard the buffer, if the buffer is full and no one there to send.
    else if (SDDS_NET_MAX_BUF_SIZE <= buf->curPos) {
    	NetBuffRef_renew(buf);
    }
    //  If the buffer is not full jet, just reset the deadline.
    else {
        buf->sendDeadline = 0;
    }
#else
	//  If latencyBudget is not active, discard the message right away.
    else {
		NetBuffRef_renew(buf);
    }
#endif

    return SDDS_RT_OK;
#ifdef SDDS_QOS_LATENCYBUDGET
}
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
