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
 * @file      Management.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#if 0

struct PubMng_t {

    // IF BUILTINTOPIC AND PUB AND LESSOFTEN
    uint8_t builtinPubCount;
    // ENDIF

};

struct SubMng_t {
    uint8_t placeholder;
};

struct Management_t {
    struct PubMng_t pub;
    struct SubMng_t sub;
    Task mngTask;

};
typedef struct Management_t* Management;

extern Management management;

/**
 * Init function for the gloal management system of sdds
 * Have to be called at the initialization of the system
 */
rc_t
Management_init(void);

#endif

#endif /* MANAGEMENT_H_ */
