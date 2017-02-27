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
 * @file      Network.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Header file for the Network abstractation
 */



#ifndef  NETWORK_H_INC
#define  NETWORK_H_INC

#include "sDDS.h"

struct Network_t;

typedef struct Network_t* Network;

rc_t
Network_init(void);

size_t
Network_size(void);

rc_t
Network_send(NetBuffRef_t* buff);

void
Network_recvFrameHandler(Network _this);

rc_t
Network_getFrameBuff(NetFrameBuff* buff);
rc_t
Network_getPayloadBegin(size_t* startByte);

rc_t
Network_createLocator(Locator_t** loc);
rc_t
Network_createMulticastLocator(Locator_t** loc);
rc_t
Network_setMulticastAddressToLocator(Locator_t* loc, char* addr);

size_t
Network_locSize(void);

rc_t
Network_set_locator_endpoint(Locator_t* loc, char* addr, int port);

rc_t
Network_setAddressToLocator(Locator_t* loc, char* addr);

rc_t
Network_getSrcAddr(char* addr, int addr_len, char* port,
                   int port_len);

rc_t
Network_setPlatformAddressToLocator(Locator_t* loc);

#endif   /* ----- #ifndef NETWORK_H_INC  ----- */
