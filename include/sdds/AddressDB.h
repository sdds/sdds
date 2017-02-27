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
 * @file      AddressDB.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Address Database class. Holds the Locator obj in the system
 */




#ifndef  ADDRESSDB_H_INC
#define  ADDRESSDB_H_INC

#include "Locator.h"

struct AddressDB_t {
    uint8_t count;
    Locator next = NULL;
};

typedef struct AddressDB_t* AddressDB;

AddressDB
AddressDB_create();
rc_t
AddressDB_add(AddressDB _this, Locator loc);
rc_t
AddressDB_del(AddressDB _this, Locator loc);

#endif   /* ----- #ifndef ADDRESSDB_H_INC  ----- */