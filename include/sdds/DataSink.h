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
 * @file      DataSink.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * data sink class to receive data. Impl all datareader of the
 */



#ifndef  DATASINK_H_INC
#define  DATASINK_H_INC

#include "sDDS.h"
#include "BuiltinTopic.h"
#include "TopicDB.h"
#include "Sample.h"
#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED
#include "FilteredDataReader.h"
#endif

struct _DataSink_t;
typedef struct _DataSink_t DataSink_t;
extern DataSink_t* dataSink;

struct DataSink_ReaderIterator {
    topicid_t iteratorTopicID;
    int8_t iteratorPos;
    int8_t iteratorNext;
    bool_t iteratorFiltered;
};
typedef struct DataSink_ReaderIterator DataSink_ReaderIterator_t;

rc_t
DataSink_init(void);

//  Obtain an instance of DataReader
DataReader_t*
DataSink_create_datareader(Topic_t* topic, Qos qos, Listener listener, StatusMask sm);

#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED
FilteredDataReader_t*
DataSink_create_filteredDatareader(LocationFilteredTopic_t* topic, Qos qos, Listener listener, StatusMask sm);
#endif
rc_t
DataSink_processFrame(NetBuffRef_t* buff);

rc_t
DataSink_printAddr();

rc_t
DataSink_getAddr(struct SNPS_Address* address);

rc_t
DataSink_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic);

rc_t
DataSink_readerIterator_reset(DataSink_ReaderIterator_t* it, topicid_t topic);

DataReader_t*
DataSink_readerIterator_next(DataSink_ReaderIterator_t* it);

rc_t
DataSink_readerIterator_hasNext(DataSink_ReaderIterator_t* it);

#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED
FilteredDataReader_t*
DataSink_getFilteredDataReaders();
#endif

#endif   /* ----- #ifndef DATASINK_H_INC  ----- */
