/*  =========================================================================
    History - Subscriber sample queue.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

/*
   @header
    History - This class queues samples of one instance for a DataReader.
   @discuss
   @end
 */

#include "sDDS.h"

static Mutex_t* mutex;
#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
static Mutex_t* blocktex;
static Task blockTask;
#endif

#ifdef SDDS_HAS_QOS_RELIABILITY
static rc_t
s_History_checkSeqNr(History_t* self, Topic_t* topic, Locator_t* loc, SDDS_SEQNR_BIGGEST_TYPE seqNr);

//#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
static rc_t
s_History_checkSeqNr_ReliableNack(History_t* self, Topic_t* topic, Locator_t* loc, SDDS_SEQNR_BIGGEST_TYPE seqNr);
//#   endif
#endif

#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
static void
s_History_block (void *data);
#endif

//  ---------------------------------------------------------------------------
//  Initializes this class

rc_t
sdds_History_init() {
    mutex = Mutex_create();
    if (mutex == NULL) {
        return SDDS_RT_FAIL;
    }
    if (Mutex_init(mutex) == SDDS_SSW_RT_FAIL) {
        return SDDS_RT_FAIL;
    }
#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
    blocktex = Mutex_create();
    if (blocktex == NULL) {
        return SDDS_RT_FAIL;
    }
    if (Mutex_init(blocktex) == SDDS_SSW_RT_FAIL) {
        return SDDS_RT_FAIL;
    }
    Mutex_lock(blocktex);
    blockTask = Task_create();
    Task_init(blockTask, s_History_block, NULL);
#endif
    return SDDS_RT_OK;
}

//  ---------------------------------------------------------------------------
//  Setup an instance of this class

rc_t
sdds_History_setup(History_t* self, Sample_t* samples, unsigned int depth) {
    assert(self);
    assert(samples);
    self->samples = samples;
    self->depth = depth;
    self->in_needle = 0;
    self->out_needle = depth;
#ifdef SDDS_HAS_QOS_RELIABILITY
    for (int index = 0; index < depth; index++){
        self->samples[index].seqNr = 0;
    }
#endif
    return SDDS_RT_OK;
}


static inline bool_t
s_History_full (History_t* self)
{
    if (self->in_needle == self->depth) {
        return true;
    }
    return false;
}


static void
s_History_block (void *data) {
    assert (data);
    Mutex_t* blocktex = (Mutex_t*) data;
	Mutex_unlock(blocktex);
}


//  ---------------------------------------------------------------------------
//  Try to enqueue a sample as buffer into the history. If the history is full
//  this call will discard the oldest sample in case of RELIABILITY best effort
//  and block in case of RELIABILITY reliable until samples are taken out. If
//  the buffer is going to be enqueued it will be decoded.
#ifdef SDDS_HAS_QOS_RELIABILITY
rc_t
sdds_History_enqueue(History_t* self, NetBuffRef_t* buff, SDDS_SEQNR_BIGGEST_TYPE seqNr) {
#else
rc_t
sdds_History_enqueue(History_t* self, NetBuffRef_t* buff) {
#endif
    assert(self);
    assert(buff);
	Mutex_lock(mutex);

#ifdef FEATURE_SDDS_TRACING_ENABLED
#   if defined (FEATURE_SDDS_TRACING_RECV_NORMAL) || defined (FEATURE_SDDS_TRACING_RECV_ISOLATED)
#       ifdef FEATURE_SDDS_TRACING_HISTORY_ENQUEUE
    Trace_point(SDDS_TRACE_EVENT_HISTORY_ENQUEUE);
#       endif
#   endif
#endif

    Topic_t* topic = buff->curTopic;
    Locator_t* loc = (Locator_t*) buff->locators->first_fn(buff->locators);
    Locator_upRef(loc);

#ifdef SDDS_HAS_QOS_RELIABILITY
    if (topic->seqNrBitSize > 0){ // topic has qos_reliability
        //  Check validity of sequence number
        if (s_History_checkSeqNr_ReliableNack(self, topic, loc, seqNr) == SDDS_RT_FAIL){
            SNPS_discardSubMsg(buff);
            Locator_downRef(loc);
            Mutex_unlock(mutex);
            return SDDS_RT_OK;
        }
    }
#endif //  End of SDDS_HAS_QOS_RELIABILITY

    if (s_History_full (self)) {
        Mutex_unlock(mutex);
#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
        //  Start timer which unblocks us after max_blocking_time
        Task_setData(blockTask, blocktex);
        Task_start(blockTask, (topic->max_blocking_time / 1000) % 60,
                               topic->max_blocking_time % 1000, SDDS_SSW_TaskMode_single);
        Mutex_lock(blocktex);
        //  Check if timer has been canceled by dequeue
        if (s_History_full (self)) {
            //  Dequeue the oldest item in the History and proceed.
            (void *) sdds_History_dequeue(self);
        }
#else
        //  Dequeue the oldest item in the History and proceed.
        (void *) sdds_History_dequeue(self);
#endif
        Mutex_lock(mutex);
    }

    //  Insert sample into queue
#ifdef SDDS_HAS_QOS_RELIABILITY
    if (topic->seqNrBitSize > 0){ // topic has qos_reliability
        self->samples[self->in_needle].seqNr = seqNr;
    }
#endif //  End of SDDS_HAS_QOS_RELIABILITY

    rc_t ret = SNPS_readData(buff, topic->Data_decode, (Data) self->samples[self->in_needle].data);
    if (ret == SDDS_RT_FAIL) {
        Mutex_unlock(mutex);
        return ret;
    }
    self->samples[self->in_needle].instance = loc;

    //  Move the input needle to the next free slot. If the input needle is at
    //  the end of the array move it to the beginning.
    unsigned int in_needle_prev = self->in_needle;
    self->in_needle++;
    if (self->in_needle >= self->depth) {
        self->in_needle = 0;
    }
    //  Move the input needle to depth to indicate that the queue is full.
    if (self->in_needle == self->out_needle) {
        self->in_needle = self->depth;
    }
    //  If the queue was previously empty set the output needle.
    if (self->out_needle >= self->depth) {
        self->out_needle = in_needle_prev;
    }
    Mutex_unlock(mutex);
    return SDDS_RT_OK;
}


//  ---------------------------------------------------------------------------
//  Takes the oldest sample out of the queue. Returns a pointer to the sample if
//  the history is not empty, otherwise NULL.

Sample_t*
sdds_History_dequeue(History_t* self) {
    assert(self);
	Mutex_lock(mutex);

    if (self->out_needle >= self->depth) {
        Mutex_unlock(mutex);
        return NULL;
    }
    //  Remove sample from queue
    Sample_t* sample = &self->samples[self->out_needle];
    Locator_downRef(sample->instance);
    //  Move the output needle to the next sample slot. If the output needle is
    //  at the end of the array move it to the beginning.
    unsigned int out_needle_prev = self->out_needle;
    self->out_needle++;
    if (self->out_needle >= self->depth) {
        self->out_needle = 0;
    }
    //  TODO: change the if statements to modulo calculations to reduce code.
    //  Move the output needle to depth to indicate that the queue is empty.
    if (self->out_needle == self->in_needle) {
        self->out_needle = self->depth;
    }
    //  If the queue was previously full set the input needle.
    if (self->in_needle >= self->depth) {
        self->in_needle = out_needle_prev;
    }

    Mutex_unlock(mutex);
#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
    if (Task_isRunning (blockTask)) {
        Task_stop (blockTask);
        Mutex_unlock (blocktex);
    }
#endif
    return sample;
}


#ifdef SDDS_HAS_QOS_RELIABILITY
static inline rc_t
s_History_checkSeqNr(History_t* self, Topic_t* topic, Locator_t* loc, SDDS_SEQNR_BIGGEST_TYPE seqNr) {
    uint8_t indexOfLoc = 0;
    bool_t isInHashmap = false;

    // check if locator is in list
    for (int index=0; index<SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS; index++){
        if (self->qos_locator[index] == loc){
            indexOfLoc = index;
            isInHashmap = true;
            break;
        }
    }

    // if not already in list, there has to be at least one free slot left
    if (!isInHashmap){
        for (int index=0; index<SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS; index++){
            if (self->qos_locator[index] == 0){
                //printf("\nHistory - new loc at: %d\n", index);
                self->highestSeqNrbyLoc[index] = seqNr;
                self->qos_locator[index] = loc;
                return SDDS_RT_OK;
            }
        }
    }

    // check the validity of the new seqNr
    switch(topic->seqNrBitSize){
        case (SDDS_QOS_RELIABILITY_SEQSIZE_BASIC):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 15)) {
                self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
                return SDDS_RT_OK;
            }
           break;
#   if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
        case (SDDS_QOS_RELIABILITY_SEQSIZE_SMALL):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 255)) {
                self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
                return SDDS_RT_OK;
            }
           break;
#   endif
#   if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
        case (SDDS_QOS_RELIABILITY_SEQSIZE_BIG):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 65536)) {
                self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
                return SDDS_RT_OK;
            }
           break;
#   endif
#   if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
        case (SDDS_QOS_RELIABILITY_SEQSIZE_HUGE):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 4294967296)) {
                self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
                return SDDS_RT_OK;
            }
           break;
#   endif
    }

    return SDDS_RT_FAIL;
}

//#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
static inline rc_t
s_History_checkSeqNr_ReliableNack(History_t* self, Topic_t* topic, Locator_t* loc, SDDS_SEQNR_BIGGEST_TYPE seqNr) {

    uint8_t indexOfLoc = 0;
    bool_t isInHashmap = false;

    // check if locator is in list
    for (int index=0; index<SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS; index++){
        if (self->qos_locator[index] == loc){
            indexOfLoc = index;
            isInHashmap = true;
            break;
        }
    }

    // if not already in list, there has to be at least one free slot left
    if (!isInHashmap){
        for (int index=0; index<SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS; index++){
            if (self->qos_locator[index] == 0){
                //printf("\nHistory - new loc at: %d\n", index);
                self->highestSeqNrbyLoc[index] = seqNr;
                self->qos_locator[index] = loc;
                return SDDS_RT_OK;
            }
        }
    }

#       if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
    if (topic->reliabilityKind == 2) { // qos_reliability is kind ACK/NACK

        if (seqNr > (self->highestSeqNrbyLoc[indexOfLoc]) ){
            uint8_t diff = (seqNr - self->highestSeqNrbyLoc[indexOfLoc]);
            //printf("diff: %d, highestSeqNrbyLoc: %d, seqNr: %d \n", diff, self->highestSeqNrbyLoc[indexOfLoc], seqNr);

            // everything's alright
            if (diff == 1){
                self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
                return SDDS_RT_OK;
            }

            // some seqNrs are missing, trying to insert in missing list
            //printf("History - missing seqNr: %d\n", seqNr);
            SDDS_SEQNR_BIGGEST_TYPE candidate = seqNr - 1;
            bool_t candidateAlreadyInList;

            for (int slot=0; slot<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; slot++) {
                if (candidate < self->highestSeqNrbyLoc[indexOfLoc]) {
                    break;
                }
                candidateAlreadyInList = 0;

                // candidate already in list?
                for (int index=0; index<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                    if (candidate == self->missingSeqNrsByLoc[indexOfLoc][index]
                    && self->missingSeqNrSlotIsUsed[indexOfLoc][index]) {
                        candidateAlreadyInList = 1;
                    }
                }

                // trying to find a free slot in list
                if (!candidateAlreadyInList){
                    for (int index=0; index<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                        if (self->missingSeqNrSlotIsUsed[indexOfLoc][index] == 0) {
                            self->missingSeqNrsByLoc[indexOfLoc][index] = candidate;
                            self->missingSeqNrSlotIsUsed[indexOfLoc][index] = 1;
                            break;
                        }
                    }
                }

                candidate--;
            }

            self->highestSeqNrbyLoc[indexOfLoc] = seqNr;
            return SDDS_RT_OK;
        }



        if (seqNr < (self->highestSeqNrbyLoc[indexOfLoc])){
//printf("  missingSeqNr: %d\n", seqNr);
            // check if received seqNr is one of the missing seqNrs in list
            for (int index=0; index < SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                if (self->missingSeqNrsByLoc[indexOfLoc][index] == seqNr
                && self->missingSeqNrSlotIsUsed[indexOfLoc][index] == 1){
                    self->missingSeqNrSlotIsUsed[indexOfLoc][index] = 0;
                    self->missingSeqNrsByLoc[indexOfLoc][index] = 0;
//printf("    missingSeqNr dequeued: %d\n", seqNr);
                    return SDDS_RT_OK;
                }
            }
        }

    } // end of qos_reliability is kind ACK/NACK
#       endif


    // check the validity of the new seqNr
    switch(topic->seqNrBitSize){
        case (SDDS_QOS_RELIABILITY_SEQSIZE_BASIC):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 15)) {
                return SDDS_RT_OK;
            }
           break;
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
        case (SDDS_QOS_RELIABILITY_SEQSIZE_SMALL):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 255)) {
                return SDDS_RT_OK;
            }
           break;
#       endif
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
        case (SDDS_QOS_RELIABILITY_SEQSIZE_BIG):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 65536)) {
                return SDDS_RT_OK;
            }
           break;
#       endif
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
        case (SDDS_QOS_RELIABILITY_SEQSIZE_HUGE):
            if ((self->highestSeqNrbyLoc[indexOfLoc] == 0)
            ||  (seqNr > self->highestSeqNrbyLoc[indexOfLoc])
            ||  (self->highestSeqNrbyLoc[indexOfLoc] == 4294967296)) {
                return SDDS_RT_OK;
            }
           break;
#       endif
    }

    return SDDS_RT_FAIL;
}
//#   endif // end of QoS_Reliability reliableNack
#endif // end of QoS_Reliability

void
sdds_History_print(History_t* self) {
    printf("History [id: %p] {\n", self);
    printf("    depth: %d,\n", self->depth);
    printf("    in needle: %d,\n", self->in_needle);
    printf("    out needle: %d,\n", self->out_needle);
    printf("    isEmpty: %d,\n", self->out_needle == self->depth);
    printf("    isFull: %d,\n", self->in_needle == self->depth);
    for (int i = 0; i < self->depth; i++)
        printf("+---");
    printf ("+\n");
    for (int i = 0; i < self->depth; i++)
        printf("| %c ", s_History_full (self) || self->out_needle < self->depth && (
                        (self->in_needle > self->out_needle && i >= self->out_needle && i < self->in_needle) ||
                        (self->in_needle < self->out_needle && (i >= self->out_needle || i < self->in_needle)))   ? 'X': ' ');
    printf ("|\n");
    for (int i = 0; i < self->depth; i++)
        printf("+---");
    printf ("+\n");
#ifdef SDDS_HAS_QOS_RELIABILITY
    printf("    samples:\n");
    for (int i=0; i<self->in_needle; i++){
        printf("        instance: %d, seqNr: %d,\n", self->samples[i].instance, self->samples[i].seqNr);
    }
#endif
    printf("}\n");
}
