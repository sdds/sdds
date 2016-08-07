/*
 * BuiltInLocationUpdateService.c
 *
 *  Created on: Aug 6, 2016
 *      Author: o_dedi
 */

#include "BuiltInLocationUpdateService.h"
#include "sDDS.h"

#ifndef SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS
#define SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS 20
#endif


static DeviceLocation_t s_trackedDevices[SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS];
static uint16_t s_deviceCount;

static void
s_updateLocation(DataReader_t* reader);

rc_t
BuiltInLocationUpdateService_init() {
    DDS_ReturnCode_t dds_ret;
    struct DDS_DataReaderListener locationListStruct = {
            .on_data_available =
            &s_updateLocation
    };
    dds_ret = DDS_DataReader_set_listener(g_DCPSLocation_reader, &locationListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set participantListener.\n");
        return SDDS_RT_FAIL;
    }
    s_deviceCount = 0;
    memset(s_trackedDevices, 0, SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS * sizeof(struct DeviceLocation));
    return SDDS_RT_OK;
}

rc_t
BuiltInLocationUpdateService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc) {

}

rc_t
BuiltInLocationUpdateService_getLocations(DeviceLocation_t* devices, uint16_t* size) {

}

static void
s_updateLocation(DataReader_t* reader) {
    SDDS_DCPSLocation loc_data_used;
    SDDS_DCPSLocation* loc_data_used_ptr = &loc_data_used;
    rc_t ret;

    do {
        ret = DDS_DCPSLocationDataReader_take_next_sample(reader, (SDDS_DCPSLocation**) &loc_data_used_ptr, NULL);
        if (ret != DDS_RETCODE_NO_DATA) {
            int devPos = s_deviceCount;
            for (int i = 0; i < s_deviceCount; i++) {
                if (s_trackedDevices[i].device == loc_data_used.device) {
                    devPos = i;
                    break;
                }
            }

            if (devPos < SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS) {
                if (devPos == s_deviceCount) {
                    s_deviceCount++;
                }
            }
        }
    } while ((ret != DDS_RETCODE_NO_DATA) && (ret != DDS_RETCODE_ERROR));
}
