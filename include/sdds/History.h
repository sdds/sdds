/*  =========================================================================
    History - Queue for DDS samples

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

//  Callbacks of this class

//  Structure of this class

struct _History_t {
	Sample_t* samples;         //  Pointer to an array of samples
	unsigned int depth;        //  Size of the queue
   unsigned int in_needle;    //  Needle that points to the next free slot
   unsigned int out_needle;   //  Needle that points to the sample to be
                              //  dequeued next
};

//  Initializes this class
rc_t
   sdds_History_init ();

//  Setup an instance of this class
rc_t
   sdds_History_setup (History_t* self, Sample_t* samples, unsigned int depth);

//  Try to enqueue a sample into the history. If the history is full this call
//  will discard the oldest sample in case of RELIABILITY best effort and block
//  in case of RELIABILITY reliable until samples are taken out.
rc_t
   sdds_History_enqueue (History_t* self, NetBuffRef_t *buff);

//  Takes the oldest sample out of the queue. Returns a pointer to the sample if
//  the history is not empty, otherwise NULL.
Sample_t*
   sdds_History_dequeue (History_t* self);


void
   sdds_History_print (History_t *self);
#ifdef __cplusplus
}
#endif

#endif
