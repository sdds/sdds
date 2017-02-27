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
 * @file      TopicDB.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * singelton class to store all the topics within the dds system on one device
 */



#ifndef  TOPICDB_H_INC
#define  TOPICDB_H_INC

bool_t
TopicDB_checkDomain(domainid_t domain);
Topic_t*
TopicDB_getTopic(topicid_t topic);
bool_t
TopicDB_checkTopic(topicid_t topic);
Topic_t*
TopicDB_createTopic(void);

void
TopicDB_iteratorReset();
Topic_t*
TopicDB_iteratorNext();
bool_t
TopicDB_iteratorHasNext();

rc_t
TopicDB_publishBuiltinTopic(NetBuffRef_t* buf);

#endif   /* ----- #ifndef TOPICDB_H_INC  ----- */
