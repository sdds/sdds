/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#include "sDDS.h"

static Task sendTask;
static Mutex_t* mutex;

//  Internal helper functions
NetBuffRef_t*
find_free_buffer(List_t* subscribers);
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
DataWriter_write(DataWriter_t* self, Data data, void* handle) {
    assert (self);
    (void) handle;

    Mutex_lock(mutex);
    Topic_t* topic = self->topic;
    List_t* subscribers = topic->dsinks.list;
    NetBuffRef_t* out_buffer = find_free_buffer(subscribers);
    Locator_t* subscriber = (Locator_t*) subscribers->first_fn(subscribers);
    while (subscriber) {
        if (Locator_contains(out_buffer->locators, subscriber) != SDDS_RT_OK) {
            out_buffer->locators->add_fn(out_buffer->locators, subscriber);
            Locator_upRef(subscriber);
        }
        subscriber = (Locator_t*) subscribers->next_fn(subscribers);
    }
    rc_t ret;
#ifdef SDDS_QOS_LATENCYBUDGET
    //  If new deadline is earlier
    if ((out_buffer->sendDeadline == 0)) {
#if SDDS_QOS_DW_LATBUD < 65536
        ret = Time_getTime16(&out_buffer->sendDeadline);
#else
        ret = Time_getTime32(&out_buffer->sendDeadline);
#endif
        out_buffer->sendDeadline += self->qos.latBudDuration;
        out_buffer->latBudDuration = self->qos.latBudDuration;
        Log_debug("sendDeadline: %d\n", out_buffer->sendDeadline);
    }
#endif

    domainid_t domain = topic->domain;
    if (out_buffer->curDomain != domain) {
        SNPS_writeDomain(out_buffer, domain);
        out_buffer->curDomain = domain;
    }

    if (out_buffer->curTopic != topic) {
        SNPS_writeTopic(out_buffer, topic->id);
        out_buffer->curTopic = topic;
    }

#ifdef SDDS_HAS_QOS_RELIABILITY
   switch(topic->reliabilityKind){
    case (SDDS_QOS_RELIABILITY_KIND_BESTEFFORT):
        if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC){
            SNPS_writeSeqNr(out_buffer, ((Reliable_DataWriter_t*)self)->seqNr);
            Log_debug("dw seqNrBasic %d\n", ((uint8_t)((Reliable_DataWriter_t*)self)->seqNr)&0x0f);
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
            SNPS_writeSeqNrSmall(out_buffer, ((Reliable_DataWriter_t*)self)->seqNr);
            Log_debug("dw seqNrSmall %d\n", (uint8_t)((Reliable_DataWriter_t*)self)->seqNr);
#endif
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
            SNPS_writeSeqNrBig(out_buffer, ((Reliable_DataWriter_t*)self)->seqNr);
            Log_debug("dw seqNrBig %d\n", (uint16_t)((Reliable_DataWriter_t*)self)->seqNr);
#endif
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE){
            SNPS_writeSeqNrHUGE(out_buffer, ((Reliable_DataWriter_t*)self)->seqNr);
            Log_debug("dw seqNrHUGE %d\n", (uint32_t)((Reliable_DataWriter_t*)self)->seqNr);
#endif
        }
    break;
#ifdef SDDS_HAS_QOS_RELIABILITY_KIND_ACK
        case (SDDS_QOS_RELIABILITY_KIND_RELIABLE_ACK):
            if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC) {
                SNPS_writeAckSeq(out_buffer, self->seqNr);
            }
            break;
#endif
#ifdef SDDS_HAS_QOS_RELIABILITY_KIND_NACK
        case (SDDS_QOS_RELIABILITY_KIND_RELIABLE_NACK):
            if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC) {
                SNPS_writeNackSeq(out_buffer, self->seqNr);
            }
            break;
#endif
    }
    ((Reliable_DataWriter_t*)self)->seqNr++;
#endif

    if (SNPS_writeData(out_buffer, topic->Data_encode, data) != SDDS_RT_OK) {
        // something went wrong oO
    	Log_error("(%d) SNPS_writeData failed\n", __LINE__);
#ifdef SDDS_QOS_LATENCYBUDGET
    	out_buffer->bufferOverflow = true;
#endif
    }

    Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);

    Mutex_unlock(mutex);
    ret = checkSending(out_buffer);
    //  Caller doen't understand different return codes but FAIL and OK
    if (ret == SDDS_RT_FAIL) {
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}
#endif // SDDS_TOPIC_HAS_SUB


#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
rc_t
DataWriter_writeAddress(DataWriter_t* self,
                        castType_t cast_type,
                        addrType_t addr_type,
                        uint8_t* addr,
                        uint8_t addr_len) {
    assert (self);
    Mutex_lock(mutex);
    Topic_t* topic = self->topic;
    domainid_t domain = topic->domain;
    List_t* subscribers = topic->dsinks.list;

    NetBuffRef_t* out_buffer = find_free_buffer(subscribers);
    Locator_t* subscriber = (Locator_t*) subscribers->first_fn(subscribers);
    while (subscriber) {
        if (Locator_contains(out_buffer->locators, subscriber) != SDDS_RT_OK) {
            if (out_buffer->locators->add_fn(out_buffer->locators, subscriber) == SDDS_RT_OK) {
            	Locator_upRef(subscriber);
            }
        }
        subscriber = (Locator_t*) subscribers->next_fn(subscribers);
    }

    if (out_buffer->curDomain != domain) {
        SNPS_writeDomain(out_buffer, domain);
        out_buffer->curDomain = domain;
    }
    if (out_buffer->curTopic != topic) {
        SNPS_writeTopic(out_buffer, topic->id);
        out_buffer->curTopic = topic;
    }

    rc_t ret;
    ret = SNPS_writeAddress(out_buffer, cast_type, addr_type, addr, addr_len);
    Mutex_unlock(mutex);
    if (ret != SDDS_RT_OK) {
        // something went wrong oO
        Log_error("Couldn't write to address\n");
        return SDDS_RT_FAIL;
    }
    Log_debug("Writing to domain %d and topic %d \n", topic->domain, topic->id);

    return SDDS_RT_OK;
}
#endif

void
checkSendingWrapper(void* buf) {
    checkSending((NetBuffRef_t*) buf);
}

rc_t
checkSending(NetBuffRef_t* buf) {
    Mutex_lock(mutex);
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

    sendDeadline = buf->sendDeadline;
    latBudDuration = buf->latBudDuration;
    bool_t overflow = buf->bufferOverflow;

    if (((time >= sendDeadline) && (time - sendDeadline < latBudDuration)) || overflow) {
        Task_stop(sendTask);
        if (overflow) {
            Log_warn("Send Data ahead of deadline due to buffer overflow.\n");
        }
        Log_debug("time: %d, deadline: %d\n", time, sendDeadline);
#endif
        // update header
        SNPS_updateHeader(buf);

        if (buf->locators->size_fn(buf->locators) > 0) {
            rc_t ret = Network_send(buf);
            if (ret != SDDS_RT_OK) {
                Log_error("Network_send failed\n");
                Mutex_unlock(mutex);
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
            Log_debug("Buffer full\n");
        }
        //  If the buffer is not full yet, just reset the deadline.
        else {
            buf->sendDeadline = 0;
            Log_debug("No subscriber\n");
        }
#else
    //  If latencyBudget is not active, discard the message right away.
    else {
        NetBuffRef_renew(buf);
    }
#endif

        Mutex_unlock(mutex);
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
            Mutex_unlock(mutex);
            return SDDS_RT_FAIL;
        }

#ifdef UTILS_DEBUG
        Log_debug("Test startet, timer: %d\n", (sendDeadline - time));
        Log_debug("%d > %d\n", sendDeadline, time);
#endif
        Mutex_unlock(mutex);
        return SDDS_RT_DEFERRED;
    }
#endif
}
