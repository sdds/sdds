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
 * @file      SubscriptionManagementService.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_

#include "sDDS.h"
#include "ManagementTopic.h"

rc_t
SubscriptionManagementService_init();

#ifdef FEATURE_SDDS_LOCATION_ENABLED
rc_t
SubscriptionManagementService_evalFilteredSubscription(DeviceLocation_t* sample);
#endif

rc_t
SubscriptionManagementService_handleParticipant(SDDS_DCPSParticipant* sample);

rc_t
SubscriptionManagementService_handlePublication(DDS_DCPSPublication* sample);

rc_t
SubscriptionManagementService_handleSubscription(SDDS_DCPSSubscription* sample);

rc_t
SubscriptionManagementService_registerFilter(SDDS_DCPSManagement* sample);


#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_ */
