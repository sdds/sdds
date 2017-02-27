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
 * @file      ManagementTopicPublicationService.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_
#define SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_

#include "sDDS.h"
#include "ManagementTopic.h"

rc_t
ManagementTopicPublicationService_init();

rc_t
ManagementTopicPublicationService_publishManagementDirective(SDDS_DCPSManagement* data, Locator_t* addr);

#endif /* SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_ */
