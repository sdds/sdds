/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#include "sDDS.h"
#include "os-ssal/Task.h"
#include "os-ssal/Mutex.h"

static Task sendTask;
static Mutex_t* mutex;
//  Forward declarations of internal helper functions
NetBuffRef_t*
findFreeFrame(List_t* dest);
rc_t
checkSending(NetBuffRef_t* buf);
void
checkSendingWrapper(void* buf);

rc_t
DataWriter_init () {
    if (TimeMng_init() != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    sendTask = Task_create();
    if (sendTask == NULL) {
        Log_error("Task_create failed\n");
        return SDDS_RT_FAIL;
    }
    ssw_rc_t ret = Task_init(sendTask, checkSendingWrapper, NULL);
    if (ret == SDDS_SSW_RT_FAIL) {
        Log_error("Task_init failed\n");
        return SDDS_RT_FAIL;
    }
    mutex = Mutex_create();
    if (mutex == NULL) {
        Log_error("Mutex_create failed\n");
        return SDDS_RT_FAIL;
    }
    ret = Mutex_init(mutex);
    if (ret == SDDS_SSW_RT_FAIL) {
        Log_error("Mutex_init failed\n");
        return SDDS_RT_FAIL;
    }

    return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataWriter_write(DataWriter_t* self, Data data, void* waste) {
    assert (self);
    (void) waste;
    NetBuffRef_t* buffRef = NULL;
    Topic_t* topic = self->topic;
    domainid_t domain = topic->domain;
    List_t* dest = topic->dsinks.list;
    rc_t ret;

#ifdef SDDS_QOS_LATENCYBUDGET
#if SDDS_QOS_DW_LATBUD < 65536
    time16_t deadline;
    ret = Time_getTime16(&deadline);
#else
    time32_t deadline;
    rc_t ret = Time_getTime32(&deadline);
#endif

    deadline += self->qos.latBudDuration;
#endif
    buffRef = findFreeFrame(dest);

    Locator_t* loc = (Locator_t*) dest->List_first(dest);
    while (loc != NULL) {
        if (Locator_contains(buffRef->addr, loc) != SDDS_RT_OK) {
            buffRef->addr->List_add(buffRef->addr, loc);
            Locator_upRef(loc);
        }
        loc = (Locator_t*) dest->List_next(dest);
    }
//    buffRef->addr = dest;
#ifdef SDDS_QOS_LATENCYBUDGET
    //  If new deadline is earlier
    if ((buffRef->sendDeadline == 0)) {
        Mutex_lock(mutex);
        buffRef->sendDeadline = deadline;
        buffRef->latBudDuration = self->qos.latBudDuration;
        Mutex_unlock(mutex);
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

    Mutex_lock(mutex);
    ret = SNPS_writeData(buffRef, topic->Data_encode, data);
    Mutex_unlock(mutex);
    if (ret != SDDS_RT_OK) {
        // something went wrong oO
    	Log_error("(%d) SNPS_writeData failed\n", __LINE__);
#ifdef SDDS_QOS_LATENCYBUDGET
//#if SDDS_QOS_DW_LATBUD < 65536
//    	ret = Time_getTime16(&deadline);
//#else
//    	rc_t ret = Time_getTime32(&deadline);
//#endif
//    	buffRef->sendDeadline = deadline;
//    	buffRef->latBudDuration = self->qos.latBudDuration;
    	buffRef->bufferOverflow = true;
#endif
//        return SDDS_RT_FAIL;
    }

#ifdef SDDS_QOS_RELIABILITY
#if SDDS_QOS_RELIABILITY_KIND == KIND_BESTEFFORT
#if SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_NORMAL
    if (SNPS_writeSeqNr(buffRef, self->qos.seqNr) != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
#elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
    if (SNPS_writeSeqNrSmall(buffRef, self->qos.seqNr) != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
#elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_BIG
    if (SNPS_writeSeqNrBig(buffRef, self->qos.seqNr) != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
#elif SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
    if (SNPS_writeSeqNrHUGE(buffRef, self->qos.seqNr) != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
#endif
    self->qos.seqNr++;

#else
    //TODO
#endif
#endif
    Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);

    ret = checkSending(buffRef);
    return ret;
}
#endif // SDDS_TOPIC_HAS_SUB


#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
rc_t
DataWriter_writeAddress(DataWriter_t* self,
                        castType_t castType,
                        addrType_t addrType,
                        uint8_t* addr,
                        uint8_t addrLen) {
    assert (self);
    NetBuffRef_t* buffRef = NULL;
    Topic_t* topic = self->topic;
    domainid_t domain = topic->domain;
    List_t* dest = topic->dsinks.list;

    buffRef = findFreeFrame(dest);

    Locator_t* loc = (Locator_t*) dest->List_first(dest);
    while (loc != NULL) {
        if (Locator_contains(buffRef->addr, loc) != SDDS_RT_OK) {
            buffRef->addr->List_add(buffRef->addr, loc);
            Locator_upRef(loc);
        }
        loc = (Locator_t*) dest->List_next(dest);
    }

    if (buffRef->curDomain != domain) {
        SNPS_writeDomain(buffRef, domain);
        buffRef->curDomain = domain;
    }
    if (buffRef->curTopic != topic) {
        SNPS_writeTopic(buffRef, topic->id);
        buffRef->curTopic = topic;
    }

    Mutex_lock(mutex);
    rc_t ret = SNPS_writeAddress(buffRef, castType, addrType, addr, addrLen);
    Mutex_unlock(mutex);
    if (ret != SDDS_RT_OK) {
        // something went wrong oO
        Log_error("%d Couldn't write to address\n", __LINE__);
        return SDDS_RT_FAIL;
    }
    Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);

    return SDDS_RT_OK;
}
#endif

void
checkSendingWrapper(void* buf) {
    checkSending((NetBuffRef_t*) buf);
}

rc_t
checkSending(NetBuffRef_t* buf) {
#ifdef SDDS_QOS_LATENCYBUDGET
#if SDDS_QOS_DW_LATBUD < 65536
    time16_t time;
    Time_getTime16(&time);
    time16_t sendDeadline;
    time16_t latBudDuration;
#else
    time32_t time;
    Time_getTime32(&time);
    time32_t sendDeadline;
    time32_t latBudDuration;
#endif

    Mutex_lock(mutex);
    sendDeadline = buf->sendDeadline;
    latBudDuration = buf->latBudDuration;
    bool_t overflow = buf->bufferOverflow;
    Mutex_unlock(mutex);

    if ( ((time >= sendDeadline) && (time - sendDeadline < latBudDuration)) || overflow) {
        Task_stop(sendTask);
        if (overflow) {
            Log_warn("Send Data ahead of deadline due to buffer overflow.\n");
        }
        Log_debug("time: %d, deadline: %d\n", time, sendDeadline);
#endif
    // update header
    SNPS_updateHeader(buf);


    if (buf->addr->List_size(buf->addr) > 0) {
        Mutex_lock(mutex);
        rc_t ret = Network_send(buf);
        if (ret != SDDS_RT_OK) {
            Log_error("Network_send failed\n");
            Mutex_unlock(mutex);
            return SDDS_RT_FAIL;
        }
        //  If frame was sent, free the buffer.
        NetBuffRef_renew(buf);
        Mutex_unlock(mutex);
    }
    //  If latencyBudget is active, don't discard the buffer right away.
#ifdef SDDS_QOS_LATENCYBUDGET
    //  Discard the buffer, if the buffer is full and no one there to send.
    else if (SDDS_NET_MAX_BUF_SIZE <= buf->curPos) {
        Mutex_lock(mutex);
        NetBuffRef_renew(buf);
        Mutex_unlock(mutex);
        Log_debug("Buffer full\n");
    }
    //  If the buffer is not full jet, just reset the deadline.
    else {
        Mutex_lock(mutex);
        buf->sendDeadline = 0;
        Mutex_unlock(mutex);
        Log_debug("No subscriber\n");
    }
#else
    //  If latencyBudget is not active, discard the message right away.
    else {
        Mutex_lock(mutex);
        NetBuffRef_renew(buf);
        Mutex_unlock(mutex);
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
    taskTime = (sendDeadline - time);
    taskSec = taskTime / 1000;
    taskMSec = taskTime % 1000;
    ssw_rc_t ret = Task_start(sendTask, taskSec, taskMSec, SDDS_SSW_TaskMode_single);
    if (ret != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
        return SDDS_RT_FAIL;
    }

#ifdef UTILS_DEBUG
    Log_debug("Test startet, timer: %d\n", (sendDeadline - time));
    Log_debug("%d > %d\n", sendDeadline, time);
#endif
    return SDDS_RT_DEFERRED;
}
#endif
}
