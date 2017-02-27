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
 * @file      SendState.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * header for the sending state maschine
 */



#ifndef  SENDSTATE_INC
#define  SENDSTATE_INC
#include "sdds_types.h"

#define SDDS_SENDSTATE_FRESH    0
#define SDDS_SENDSTATE_ADDR     1
#define SDDS_SENDSTATE_DOMAIN   2
#define SDDS_SENDSTATE_TOPIC    3
#define SDDS_SENDSTATE_DATA     4


struct SendState_t {
    uint8_t state;
};


#endif   /* ----- #ifndef SENDSTATE_INC  ----- */