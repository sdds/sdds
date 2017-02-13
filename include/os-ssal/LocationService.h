/*
 * LocationService.h
 *
 *  Created on: Aug 3, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_OS_SSAL_LOCATIONSERVICE_H_
#define SDDS_INCLUDE_OS_SSAL_LOCATIONSERVICE_H_

#include "Geometry.h"
#include "os-ssal/NodeConfig.h"
#include "os-ssal/TimeMng.h"
#include "os-ssal/SSW.h"

struct DeviceLocation {
    SSW_NodeID_t device;
    Ellipse_t area;
    time16_t time;
    msec16_t expiration;
};
typedef struct DeviceLocation DeviceLocation_t;

ssw_rc_t
LocationService_init();

ssw_rc_t
LocationService_getLocation(DeviceLocation_t* devLoc);

ssw_rc_t
LocationService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc);

#endif /* SDDS_INCLUDE_OS_SSAL_LOCATIONSERVICE_H_ */
