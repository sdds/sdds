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
 * @file      NetBuffRef.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * class to wrap the networkbuffer and store the state of the
 */



#ifndef  NETBUFFREF_H_INC
#define  NETBUFFREF_H_INC

#include "List.h"
#include "sDDS.h"

struct Locator_t;

struct _NetBuffRef_t {
    uint8_t subMsgCount;

    NetFrameBuff frame_start;
    byte_t* buff_start;
    uint16_t curPos;

    // QOS part
#if SDDS_QOS_DW_LATBUD < 65536
    time16_t sendDeadline;
    time16_t latBudDuration;
#else
    time32_t sendDeadline;
    time32_t latBudDuration;
#endif
    bool_t bufferOverflow : 1;


    // state of the frame
    List_t* locators;
    Topic_t* curTopic;
    domainid_t curDomain;
};

rc_t
NetBuffRef_init(NetBuffRef_t* _this);
rc_t
NetBuffRef_renew(NetBuffRef_t* _this);

//#ifdef UTILS_DEBUG
void
NetBuffRef_print(NetBuffRef_t* _this);

void
NetBuffRef_print_subMsgs (NetBuffRef_t* _this);

void
NetBuffRef_print_subMsgType(NetBuffRef_t* _this, subMsg_t type);
//#endif
#endif   /* ----- #ifndef NETBUFFREF_H_INC  ----- */
