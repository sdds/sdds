/*
 * =====================================================================================
 *
 *       Filename:  DataSink.c
 *
 *    Description:  Impl file for the DataSink class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 08:57:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "sDDS.h"
#include <os-ssal/Trace.h>

struct _DataSink_t {
    DataReader_t readers[SDDS_DATA_READER_MAX_OBJS];
    uint64_t allocated_readers;
    SNPS_Address_t addr;
};
static DataSink_t _dataSink;
static DataSink_t* self = &_dataSink;

//  Forward declarations of internal helper functions

rc_t
checkDomain(NetBuffRef_t* buff);
rc_t
checkTopic(NetBuffRef_t* buff, topicid_t topic);
rc_t
BuiltinTopicDataReader_encode(NetBuffRef_t* buff, Data data, size_t* size);


//  ---------------------------------------------------------------------------
//  Initialize this class

rc_t
DataSink_init(void) {
    return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSink_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic) {
    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        if (  BitArray_check(&self->allocated_readers, index)
           &&  DataReader_topic(&self->readers[index])->id == id) {
            if (st != NULL) {
                st->key = DataReader_id(&self->readers[index]);
                st->participant_key = BuiltinTopic_participantID;
                st->topic_id = DataReader_topic(&self->readers[index])->id;
            }
            if (topic != NULL) {
                *topic = DataReader_topic(&self->readers[index]);
            }
            return SDDS_RT_OK;
        }
    }
    Log_debug("No DataReader found that listens for topics with id %d\n", id);
    return SDDS_RT_FAIL;
}
#endif


DataReader_t*
DataSink_DataReader_by_topic(topicid_t id) {
    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        if (  BitArray_check(&self->allocated_readers, index)
           &&  DataReader_topic(&self->readers[index])->id == id) {
            return &self->readers[index];
        }
    }
    return NULL;
}


rc_t
DataSink_getAddr(SNPS_Address_t* address) {
    address->addrType = self->addr.addrType;
    address->addrCast = self->addr.addrCast;
    address->addr = self->addr.addr;
    return SDDS_RT_OK;
}

//  ---------------------------------------------------------------------------
//  Processes a SNPS message by reading through all submessages. Will enqueue a
//  Sample into a DataReader's History. Returns OK if successful, otherwise
//  FAIL.
rc_t
DataSink_processFrame(NetBuffRef_t* buff) {
    assert(buff);
#ifdef FEATURE_SDDS_TRACING_ENABLED
#ifdef FEATURE_SDDS_TRACING_PROCESS_FRAME
    Trace_point(SDDS_TRACE_EVENT_PROCESS_FRAME);
#endif
#endif
    //  Parse the header
    rc_t ret;
    ret = SNPS_readHeader(buff);
    if (ret == SDDS_RT_FAIL) {
        Log_error("Invalid SNPS header\n");
        return ret;
    }

    topicid_t topic_id;
#ifdef SDDS_HAS_QOS_RELIABILITY
    SDDS_SEQNR_BIGGEST_TYPE seqNr = 0;
#endif

    while (buff->subMsgCount > 0) {
        subMsg_t type;
        SNPS_evalSubMsg(buff, &type);
        switch (type) {
        case (SDDS_SNPS_T_DOMAIN):
            ret = checkDomain(buff);
            if (ret == SDDS_RT_FAIL) {
                Log_error("Invalid domain!\n");
                return ret;
            }
            break;
        case (SDDS_SNPS_T_TOPIC):
            ret = SNPS_readTopic(buff, &topic_id);
            if (ret == SDDS_RT_FAIL) {
                Log_error("Can't read Topic\n");
                return ret;
            }
            Log_debug("Read topic %d\n", topic_id);
            checkTopic(buff, topic_id);
            break;
        case (SDDS_SNPS_T_DATA):
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
            {
                DataReader_t* data_reader = DataSink_DataReader_by_topic(topic_id);
                if (data_reader == NULL) {
                    Log_debug("Couĺdn't get Data Reader for topic id %d: "
                              "Discard submessage\n", topic_id);
                    SNPS_discardSubMsg(buff);
                    return SDDS_RT_FAIL;
                }
                History_t* history = DataReader_history(data_reader);
#ifdef SDDS_HAS_QOS_RELIABILITY
                ret = sdds_History_enqueue_buffer(history, buff, seqNr);
#else
                ret = sdds_History_enqueue_buffer(history, buff);
#endif
                if (ret == SDDS_RT_FAIL) {
                    Log_warn("Can't parse data: Discard submessage\n");
                    SNPS_discardSubMsg(buff);
                    return SDDS_RT_FAIL;
                }
            }
#endif
            break;
        case (SDDS_SNPS_T_ADDRESS):
            //  Write address into global variable
            if (SNPS_readAddress(buff, &self->addr.addrCast, &self->addr.addrType, &self->addr.addr) != SDDS_RT_OK) {
                Log_warn("Read address failed, discard subMessage.\n", type);
                SNPS_discardSubMsg(buff);
            }
            break;
        case (SDDS_SNPS_T_TSSIMPLE):
        case (SDDS_SNPS_T_STATUS):

#ifdef SDDS_HAS_QOS_RELIABILITY
        case (SDDS_SNPS_T_SEQNR):
            SNPS_readSeqNr(buff, (uint8_t*) &seqNr);
            break;
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
        case (SDDS_SNPS_T_SEQNRSMALL):
            SNPS_readSeqNrSmall(buff, (uint8_t*) &seqNr);
            break;
#endif
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
        case (SDDS_SNPS_T_SEQNRBIG):
            SNPS_readSeqNrBig(buff, (uint16_t*) &seqNr);
            break;
#endif
#if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
        case (SDDS_SNPS_T_SEQNRHUGE):
            SNPS_readSeqNrHUGE(buff, (uint32_t*) &seqNr);
            break;
#endif
#endif
        default:
            //  Go to next submessage
            Log_warn("Invalid submessage type %d: Discard submessage\n", type);
            SNPS_discardSubMsg(buff);
            break;
        }
    }
    // Reset the buffer
//    NetBuffRef_renew(buff);

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
    // Send event notifications
    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        DataReader_t* data_reader = &self->readers[index];
        if (!data_reader) {
            continue;
        }
        int tpc = DataReader_topic(data_reader)->id;
        if ((topic_id == tpc) && DataReader_on_data_avail_listener(data_reader)) {
            On_Data_Avail_Listener on_data_avail_listener =
                DataReader_on_data_avail_listener(data_reader);
            //  Notify listener
            on_data_avail_listener(data_reader);
        }
    }
#endif
    return SDDS_RT_OK;
}


#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
//  ---------------------------------------------------------------------------
//  Creates a new DataReader object

DataReader_t*
DataSink_create_datareader(Topic_t* topic, Qos qos, Listener listener, StatusMask sm) {
    (void) qos;
    (void) sm;

    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        //  Check if object at index has been allocated
        if (!BitArray_check(&self->allocated_readers, index)) {
            //  Allocate object at index
            BitArray_set(&self->allocated_readers, index);
            DataReader_t* reader = &self->readers[index];
            // Initialize object properties
            reader->id = index;
            reader->topic = topic;
            reader->on_data_avail_listener = NULL;
            Log_debug("Create data reader with id %d\n", DataReader_id(reader));
            return reader;
        }
    }
    return NULL;
}
#endif


rc_t
checkDomain(NetBuffRef_t* buff) {
    domainid_t domain;
    SNPS_readDomain(buff, &domain);
    if (TopicDB_checkDomain(domain) == false) {
        SNPS_gotoNextSubMsg(buff, SDDS_SNPS_T_DOMAIN);
    }
    else {
        buff->curDomain = domain;
    }
    return SDDS_RT_OK;
}

rc_t
checkTopic(NetBuffRef_t* buff, topicid_t topic) {
    Topic_t* t_ptr = TopicDB_getTopic(topic);
    if (t_ptr == NULL) {
        SNPS_gotoNextSubMsg(buff, SDDS_SNPS_T_TOPIC);
    }
    else {
        buff->curTopic = t_ptr;
    }

    return SDDS_RT_OK;
}

rc_t
BuiltinTopic_writeDataReaders2Buf(NetBuffRef_t* buf) {
    SNPS_writeTopic(buf, DDS_DCPS_SUBSCRIPTION_TOPIC);
    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        SNPS_writeData(buf, BuiltinTopicDataReader_encode,
                       (Data) &self->readers[index]);
    }

    return SDDS_RT_OK;
}
rc_t
BuiltinTopicDataReader_encode(NetBuffRef_t* buff, Data data, size_t* size) {
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
    DataReader_t* dr = (DataReader_t*) data;
    byte_t* start = buff->buff_start + buff->curPos;
    *size = 0;
    Marshalling_enc_uint8(start + (*size), &(DataReader_topic(dr)->domain));
    *size += sizeof(domainid_t);
    Marshalling_enc_uint8(start + (*size), &(DataReader_topic(dr)->id));
    *size += sizeof(topicid_t);
#endif

    return SDDS_RT_OK;
}
