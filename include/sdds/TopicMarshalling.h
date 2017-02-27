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
 * @file      TopicMarshalling.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * interface for the marshalling of the data assigend to a
 */



#ifndef  TOPICMARSHALLING_H_INC
#define  TOPICMARSHALLING_H_INC

#ifdef SDDS_PLATFORM_autobest
#ifndef stderr
#define stderr 2
#endif
#define fprintf(a, b, args ...) printf(b, ## args)
#endif

#include "sdds_types.h"

typedef rc_t (TopicMarshalling_encode_fn) (NetBuffRef_t* buff, Data data, size_t* size);
typedef rc_t (TopicMarshalling_decode_fn) (NetBuffRef_t* buff, Data data, size_t* size);

#endif   /* ----- #ifndef TOPICMARSHALLING_H_INC  ----- */
