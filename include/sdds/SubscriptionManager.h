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
 * @file      SubscriptionManager.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_

#include "sDDS.h"
#include "SubscriptionGraph.h"
#include "FilterEvaluator.h"
#include "ManagementTopic.h"

struct SubscriptionManager {
    SubscriptionGraph_t subscriptionGraph;
};
typedef struct SubscriptionManager SubscriptionManager_t;

rc_t
SubscriptionManager_init(SubscriptionManager_t* self);

rc_t
SubscriptionManager_evalFilteredSubscription(SubscriptionManager_t* self, DeviceLocation_t* sample);

rc_t
SubscriptionManager_publishSubscriptionState(DirectedEdge_t* edge);

rc_t
SubscriptionManager_handleParticipant(SubscriptionManager_t* self, SDDS_DCPSParticipant* sample);

rc_t
SubscriptionManager_handlePublication(SubscriptionManager_t* self, DDS_DCPSPublication* sample);

rc_t
SubscriptionManager_handleSubscription(SubscriptionManager_t* self, SDDS_DCPSSubscription* sample);

rc_t
SubscriptionManager_registerFilter(SubscriptionManager_t* self, SDDS_DCPSManagement* sample);

#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_ */
