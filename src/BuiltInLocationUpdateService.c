/*
 * BuiltInLocationUpdateService.c
 *
 *  Created on: Aug 6, 2016
 *      Author: o_dedi
 */

#include "BuiltInLocationUpdateService.h"

#ifndef SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS
#define SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS 20
#endif


static DeviceLocation_t s_trackedDevices[SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS];
static uint16_t s_deviceCount;

static void
updateLocation(DataReader_t* reader);

rc_t
BuiltInLocationUpdateService_init() {

}

rc_t
BuiltInLocationUpdateService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc) {

}

rc_t
BuiltInLocationUpdateService_getLocations(DeviceLocation_t* devices, uint16_t* size) {

}
