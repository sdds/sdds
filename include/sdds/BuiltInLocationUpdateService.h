/*
 * BuiltInLocationUpdateService.h
 *
 *  Created on: Aug 6, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_BUILTINLOCATIONUPDATESERVICE_H_
#define SDDS_INCLUDE_SDDS_BUILTINLOCATIONUPDATESERVICE_H_

#include "sDDS.h"
#include "os-ssal/LocationService.h"

rc_t
BuiltInLocationUpdateService_init();

rc_t
BuiltInLocationUpdateService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t** devLoc);

rc_t
BuiltInLocationUpdateService_getLocations(DeviceLocation_t** devices, uint16_t* size);

#endif /* SDDS_INCLUDE_SDDS_BUILTINLOCATIONUPDATESERVICE_H_ */
