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
 * @file      LocationTrackingService.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_OS_SSAL_LOCATIONTRACKINGSERVICE_H_
#define SDDS_INCLUDE_OS_SSAL_LOCATIONTRACKINGSERVICE_H_

#include "List.h"
#include "os-ssal/LocationService.h"

rc_t
LocationTrackingService_init();

rc_t
LocationTrackingService_trackDevice(SSW_NodeID_t device);

rc_t
LocationTrackingService_untrackDevice(SSW_NodeID_t device);

rc_t
LocationTrackingService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc);

List_t*
LocationTrackingService_getLocations();

#endif /* SDDS_INCLUDE_OS_SSAL_LOCATIONTRACKINGSERVICE_H_ */
