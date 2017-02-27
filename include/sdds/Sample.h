/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      Sample.h
 * @author    TODO
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * DDS sample representation
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
