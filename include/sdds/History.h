/*  =========================================================================
    History - Subscriber sample queue.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#ifndef __HISTORY_H_INCLUDED__
#define __HISTORY_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Structure of this class

struct _History_t {
    Sample_t* samples;        //  Pointer to an array of samples
    uint16_t depth;           //  Size of the queue
    uint16_t in_needle;       //  Needle that points to the next free slot
    uint16_t out_needle;      //  Needle that points to the sample to be
                              //  dequeued next
    Mutex_t* mutex;
#ifdef SDDS_HAS_QOS_RELIABILITY
    Locator_t* qos_locator[SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS];
    SDDS_SEQNR_BIGGEST_TYPE highestSeqNrbyLoc[SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS];
#   if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
    || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
    SDDS_SEQNR_BIGGEST_TYPE missingSeqNrsByLoc[SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS][SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE];
    bool_t missingSeqNrSlotIsUsed [SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS][SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE];
     Mutex_t* blocktex;
     Task blockTask;
#   endif

#endif
};

//  Initializes this class
rc_t
sdds_History_init();

//  Setup an instance of this class
rc_t
sdds_History_setup(History_t* self, Sample_t* samples, unsigned int depth);

//  Try to enqueue a sample into the history. If the history is full this call
//  will discard the oldest sample in case of RELIABILITY best effort and block
//  in case of RELIABILITY reliable until samples are taken out.
#ifdef SDDS_HAS_QOS_RELIABILITY
rc_t
sdds_History_enqueue(History_t* self, NetBuffRef_t* buff, SDDS_SEQNR_BIGGEST_TYPE seqNr);
#else
rc_t
sdds_History_enqueue(History_t* self, NetBuffRef_t* buff);
#endif

//  Takes the oldest sample out of the queue. Returns a pointer to the sample if
//  the history is not empty, otherwise NULL.
Sample_t*
sdds_History_dequeue(History_t* self);

#ifdef UTILS_DEBUG
void
sdds_History_print(History_t* self);
#endif

#ifdef __cplusplus
}
#endif

#endif
