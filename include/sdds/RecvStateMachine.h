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
 * @file      RecvStateMachine.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Class for the parsing of frames received.
 */



#ifndef  RECVSTATEMACHINE_H_INC
#define  RECVSTATEMACHINE_H_INC

#include "sdds_types.h"

rc_t
FreshFrame_readSubMsg(NetBuffRef buff);
rc_t
DomainSet_readSubMsg(NetBuffRef buff);
rc_t
TopicSet_readSubMsg(NetBuffRef buff);
rc_t
DataSet_readSubMsg(NetBuffRef buff);

#endif   /* ----- #ifndef RECVSTATEMACHINE_H_INC  ----- */