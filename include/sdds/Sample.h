/*  =========================================================================
    Sample - DDS sample representation

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#ifndef __SAMPLE_H_INCLUDED__
#define __SAMPLE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Callbacks of this class

//  Structure of this class

struct _Sample_t {
    void* data;       //  Sample data
    Locator_t* instance;
#if defined SDDS_HAS_QOS_RELIABILITY
    SDDS_SEQNR_BIGGEST_TYPE seqNr;
#endif
};

#if defined SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK || defined SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
struct _ReliableSample_t {
    void* data;       //  Sample data
    SDDS_SEQNR_BIGGEST_TYPE seqNr;
    time32_t timeStamp;
    bool_t isUsed;
};
#endif

#ifdef __cplusplus
}
#endif

#endif
