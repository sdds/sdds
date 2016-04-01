/*  =========================================================================
    DataWriter - DDS DataWriter representation, allows the application to
                 declare the data it wishes to send.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#ifndef __DATA_WRITER_H_INCLUDED__
#define __DATA_WRITER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef TEST_SCALABILITY
#   ifndef SCALABILITY_LOG
#define SCALABILITY_LOG "../scalability_msg_count.log"
#   endif
#endif

//  Structure of this class

struct _DataWriter_t {
    Topic_t* topic;
    SourceQos_t qos;
    unsigned int id : 4;
};
typedef struct _DataWriter_t DataWriter_t;

#if defined SDDS_HAS_QOS_RELIABILITY
struct _Reliable_DataWriter_t {
   DataWriter_t dw;
   SDDS_SEQNR_BIGGEST_TYPE seqNr;

#   if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
    ReliableSample_t* samplesToKeep;
    uint8_t depthToKeep;
#   endif
};
typedef struct _Reliable_DataWriter_t Reliable_DataWriter_t;
#endif

rc_t
DataWriter_init();

#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
rc_t
DataWriter_setup(Reliable_DataWriter_t* self, ReliableSample_t* samples, unsigned int depth);
#endif

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
rc_t
DataWriter_write(DataWriter_t* self, Data data, void* handle);
#endif// SDDS_TOPIC_HAS_SUB

NetBuffRef_t*
find_free_buffer(List_t* subscribers);

rc_t
checkSending(NetBuffRef_t* buf);

rc_t
DataWriter_mutex_lock();

rc_t
DataWriter_mutex_unlock();

rc_t
DataWriter_writeAddress(DataWriter_t* self,
                        castType_t castType,
                        addrType_t addrType,
                        uint8_t* addr,
                        uint8_t addrLen);

#ifdef __cplusplus
}
#endif

#endif
