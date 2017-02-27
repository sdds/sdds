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
 * @file      BuiltInTopicPublicationService.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_
#define SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_

#include "sDDS.h"

rc_t
BuiltInTopicPublicationService_init();

rc_t
BuiltInTopicPublicationService_publishDCPSParticipant();

rc_t
BuiltInTopicPublicationService_publishDCPSTopic(topicid_t id);

rc_t
BuiltInTopicPublicationService_publishDCPSPublication();

rc_t
BuiltInTopicPublicationService_publishDCPSSubscription(topicid_t id);

#ifdef FEATURE_SDDS_LOCATION_ENABLED
rc_t
BuiltInTopicPublicationService_publishDCPSLocation(DeviceLocation_t* dev);
#endif

#endif /* SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_ */
