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

//  Structure of this class

struct _DataWriter_t {
    Topic_t* topic;
    SourceQos_t qos;
    unsigned int id : 4;
    History_t* history;
};
typedef struct _DataWriter_t DataWriter_t;

#if defined SDDS_HAS_QOS_RELIABILITY
struct Reliable_DataWriter {
   struct _DataWriter_t* dw;
   SDDS_SEQNR_BIGGEST_TYPE seqNr;
};
typedef struct Reliable_DataWriter Reliable_DataWriter_t;
#endif


rc_t
DataWriter_init();

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataWriter_write(DataWriter_t* self, Data data, void* waste);
#endif// SDDS_TOPIC_HAS_SUB

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
