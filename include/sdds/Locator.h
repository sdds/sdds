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
 * @file      Locator.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Locator interface for the abstractaion of the network address
 */



#ifndef  LOCATOR_H_INC
#define  LOCATOR_H_INC
#include "sdds_types.h"
#include "List.h"

#define SDDS_LOCATOR_STATE_EMPTY        0x00
#define SDDS_LOCATOR_STATE_FILLED       0x01
#define SDDS_LOCATOR_STATE_REF          0x02

#define SDDS_LOCATOR_TYPE_UNI           0x00
#define SDDS_LOCATOR_TYPE_MULTI         0x01
#define SDDS_LOCATOR_TYPE_BROAD         0x02

// typedef struct _Locator_t Locator_t;
struct _Locator_t {
    // TODO state reicht nicht ref counter?
    // wie geht das mit den bitmaps?
    // 4 bit refconter?
    unsigned int type   : 2;
    _Bool isEmpty       : 1;
    _Bool isDest        : 1;
    _Bool isSender      : 1;
    uint8_t refCount;
};


/**
 * Checks if two Locator instances are equal or the same.
 * There is no distinction between these two cases.
 * The comparison depends on the specific implementation.
 * This method is abstract and have to be implemented in the
 * network specific class, like the "network module"
 *
 * @param l1 First Locator instance
 * @param l2 Second Locator instance
 * @return true or false, dependig if the two instances are equal or not
 */
bool_t
Locator_isEqual(Locator_t* l1, Locator_t* l2);

/**
 * Increments the reference counter of the locator instance.
 * @param _this Poniter to the Locator instance
 */
void
Locator_upRef(Locator_t* _this);


/**
 * decrements the reference counter of the locator instance.
 * If the counter becomes zero, the instance will be freed in the LocatorDB.
 *
 * @param _this Locator instance, which refcount shall be reduced
 */
void
Locator_downRef(Locator_t* _this);

rc_t
Locator_contains(List_t* list, Locator_t* l);

rc_t
Locator_getAddress(Locator_t* l, char* srcAddr, size_t max_addr_len);

rc_t
Locator_copy(Locator_t* src, Locator_t* dst);

rc_t
Locator_print(Locator_t* loc);

#endif   /* ----- #ifndef LOCATOR_H_INC  ----- */
