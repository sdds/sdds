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
 * @file      LocatorDB.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * LocatorDB class manages the Locator object within the
 */


#ifndef  LOCATORDB_H_INC
#define  LOCATORDB_H_INC

struct LocatorDB_t;
typedef struct LocatorDB_t* LocatorDB;

#define AbstractAddressField void*

#define SDDS_LOCATORDB_RT_FOUND 4
#define SDDS_LOCATORDB_RT_NOTFOUND 5
#define SDDS_LOCATORDB_RT_NOFREELOCATORS 6
#define SDDS_LOCATORDB_RT_ISINUSE 7



rc_t
LocatorDB_init(void);

rc_t
LocatorDB_newLocator(Locator_t** loc);
rc_t
LocatorDB_findLocator(Locator_t* toFind, Locator_t** loc);
rc_t
LocatorDB_isUsedLocator(Locator_t* loc);
rc_t
LocatorDB_newMultiLocator(Locator_t** loc);
rc_t
LocatorDB_findLocatorByMcastAddr(char *addr, Locator_t** result);
rc_t
LocatorDB_findLocatorByAddr(char *addr, Locator_t** result);
uint8_t
LocatorDB_freeLocators();
void
LocatorDB_print();

#endif   /* ----- #ifndef LOCATORDB_H_INC  ----- */
