/*
 * LocationService.c
 *
 *  Created on: Aug 4, 2016
 *      Author: o_dedi
 */

#include "os-ssal/LocationService.h"
#include <string.h>
#include "sDDS.h"

#ifdef FEATURE_SDDS_LOCATION_ENABLED
#   ifdef FEATURE_SDDS_SIMULATION_ENABLED

extern DeviceLocation_t* g_LocationService_simulatedDevices;
extern uint16_t g_LocationService_simulatedDevicesCount;

static SSW_NodeID_t s_deviceID;

ssw_rc_t
LocationService_init() {
    s_deviceID = NodeConfig_getNodeID();
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
LocationService_getLocation(DeviceLocation_t* devLoc) {
    return LocationService_getDeviceLocation(s_deviceID, devLoc);
}

ssw_rc_t
LocationService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc) {
    assert(device);
    assert(devLoc);

    for (uint16_t i = 0; i < g_LocationService_simulatedDevicesCount; i++ ) {
        if (g_LocationService_simulatedDevices[i].device == device) {
            memcpy(devLoc, &g_LocationService_simulatedDevices[i], sizeof(struct DeviceLocation));
            return SDDS_SSW_RT_OK;
        }
    }

    return SDDS_SSW_RT_FAIL;
}

#   else

ssw_rc_t
LocationService_init() {
    return SDDS_SSW_RT_FAIL;
}

ssw_rc_t
LocationService_getLocation(DeviceLocation_t* devLoc) {
    return SDDS_SSW_RT_FAIL;
}

ssw_rc_t
LocationService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc) {
    return SDDS_SSW_RT_FAIL;
}

#   endif
#endif
