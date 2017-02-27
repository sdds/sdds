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
 * @file      DataSource.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Header File for the DataSource class. Object is dedicated to
 * handle the publishing of data to the dds network.
 */



#ifndef  DATASOURCE_H_INC_
#define  DATASOURCE_H_INC_
#include "sDDS.h"

#define SDDS_DATASOURCE_OK 0

struct _DataSource_t;
typedef struct _DataSource_t DataSource_t;

extern DataSource_t* dataSource;

rc_t
DataSource_init(void);

DataWriter_t*
DataSource_create_datawriter(Topic_t* topic, Qos qos, Listener list, StatusMask mask);

rc_t
DataSource_writeAddress(DataWriter_t* _this, castType_t castType, addrType_t addrType, uint8_t* addr, uint8_t addrLen);

#if defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSource_getDataWrites(DDS_DCPSPublication* pt, int* len);
#endif

#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
Reliable_DataWriter_t*
DataSource_DataWriter_by_topic(topicid_t id);
#endif

#if defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSource_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic);
#endif

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
rc_t
DataSource_write(DataWriter_t* _this, Data data, void* waste);
#endif// SDDS_TOPIC_HAS_SUB

#ifdef UTILS_DEBUG
void print_Pointer();
#endif

#endif   /* ----- #ifndef DATASOURCE_H_INC  ----- */
