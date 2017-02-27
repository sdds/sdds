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
 * @file      LocationFilteredTopic.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_
#define SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_

#include "sDDS.h"
#include "ContentFilteredTopic.h"
#include "GeometryStore.h"
#include "os-ssal/LocationService.h"
#include "sdds_types.h"


#ifndef SDDS_FILTER_EXPR_MAX_LEN
#define SDDS_FILTER_EXPR_MAX_LEN 32
#endif

enum ConnectorType {
    CONNECTOR_NONE,
    CONNECTOR_AND,
    CONNECTOR_OR
};
typedef enum ConnectorType ConnectorType_t;

struct FilterExpression {
    bool_t negation;
    uint8_t functionID;
    uint8_t geometryID;
};
typedef struct FilterExpression FilterExpression_t;

struct FilterState {
    bool_t result;
    ConnectorType_t connector;
    uint8_t currentPosition;
    FilterExpression_t expression;
};
typedef struct FilterState FilterState_t;

struct LocationFilteredTopic {
    ContentFilteredTopic_t contentFilteredTopic;
    uint8_t filterExpression[SDDS_FILTER_EXPR_MAX_LEN];
    uint8_t expressionLength;
    FilterState_t filterstate;
    GeometryStore_t* geometryStore;
};
typedef struct LocationFilteredTopic LocationFilteredTopic_t;

rc_t
LocationFilteredTopic_create(LocationFilteredTopic_t* self, Topic_t* topic, char* filterExpression, GeometryStore_t* store);

rc_t
LocationFilteredTopic_setFilter(LocationFilteredTopic_t* self, char* filterExpression);

rc_t
LocationFilteredTopic_evalSample(LocationFilteredTopic_t* self, Data data);

rc_t
LocationFilteredTopic_evalExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc);

rc_t
LocationFilteredTopic_equals(LocationFilteredTopic_t* t1, LocationFilteredTopic_t* t2);


#endif /* SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_ */
