/*  =========================================================================
    History - Queue for DDS samples

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

/*
@header
    History - This class queues samples of one instance for either a DataReader
              or a DataWriter.
@discuss
@end
*/

#include "sDDS.h"


//  ---------------------------------------------------------------------------
//  Initializes this class

rc_t
sdds_History_init ()
{
    return SDDS_RT_OK;
}

//  ---------------------------------------------------------------------------
//  Setup an instance of this class

rc_t
sdds_History_setup (History_t* self, Sample_t* samples, unsigned int depth)
{
    assert (self);
    assert (samples);
    self->samples = samples;
    self->depth = depth;
    self->in_needle = 0;
    self->out_needle = depth;
    return SDDS_RT_OK;
}


//  ---------------------------------------------------------------------------
//  Try to enqueue a sample into the history. If the history is full this call
//  will discard the oldest sample in case of RELIABILITY best effort and block
//  in case of RELIABILITY reliable until samples are taken out.

rc_t
sdds_History_enqueue (History_t* self, NetBuffRef_t *buff)
{
    assert (self);
    assert (buff);
    //  Queue is full.
    if (self->in_needle == self->depth) {
        return SDDS_RT_FAIL;
    }
    //  Insert sample into queue
	Topic_t *topic = buff->curTopic;
    SNPS_readData (buff, topic->Data_decode, (Data) self->samples[self->in_needle].data);
    self->samples[self->in_needle].instance = buff->addr;
    //  Move the input needle to the next free slot. If the input needle is at
    //  the end of the array move it to the beginning.
    unsigned int in_needle_prev = self->in_needle;
    self->in_needle++;
    if (self->in_needle == self->depth) {
        self->in_needle = 0;
    }
    //  Move the input needle to depth to indicate that the queue is full.
    if (self->in_needle == self->out_needle) {
        self->in_needle = self->depth;
    }
    //  If the queue was previously empty set the output needle.
    if (self->out_needle == self->depth) {
        self->out_needle = in_needle_prev;
    }
    return SDDS_RT_OK;
}


//  ---------------------------------------------------------------------------
//  Takes the oldest sample out of the queue. Returns a pointer to the sample if
//  the history is not empty, otherwise NULL.

Sample_t*
sdds_History_dequeue (History_t* self)
{
    assert (self);
    //  Queue is empty.
    if (self->out_needle == self->depth) {
        return NULL;
    }
    //  Remove sample from
    Sample_t *sample = &self->samples[self->out_needle];
    //  Move the output needle to the next sample slot. If the output needle is
    //  at the end of the array move it to the beginning.
    unsigned int out_needle_prev = self->out_needle;
    self->out_needle++;
    if (self->out_needle == self->depth) {
        self->out_needle = 0;
    }
    //  Move the output needle to depth to indicate that the queue is empty.
    if (self->out_needle == self->in_needle) {
        self->out_needle = self->depth;
    }
    //  If the queue was previously full set the input needle.
    if (self->in_needle == self->depth) {
        self->in_needle = out_needle_prev;
    }
    return sample;
}

void
sdds_History_print (History_t *self)
{
    printf ("History {\n");
    printf ("    depth: %d,\n", self->depth);
    printf ("    in needle: %d,\n", self->in_needle);
    printf ("    out needle: %d,\n", self->out_needle);
    printf ("}\n");
}


