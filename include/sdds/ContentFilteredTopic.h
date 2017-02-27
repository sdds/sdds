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
 * @file      ContentFilteredTopic.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_
#define SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_

#include "sDDS.h"
#include "Topic.h"

struct ContentFilteredTopic {
    Topic_t* associatedTopic;
};
typedef struct ContentFilteredTopic ContentFilteredTopic_t;



#endif /* SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_ */
