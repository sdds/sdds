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
    SDDS_SEQNR_BIGGEST_TYPE seqNr;
};

#ifdef __cplusplus
}
#endif

#endif
