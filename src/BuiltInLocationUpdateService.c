/*
 * BuiltInLocationUpdateService.c
 *
 *  Created on: Aug 6, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "BuiltInLocationUpdateService.h"
#include "BuiltinTopic.h"
#include "Geometry.h"
#include "SubscriptionManagementService.h"
#include "dds/DDS_DCPS.h"

#if defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) && defined(FEATURE_SDDS_LOCATION_ENABLED)

# ifndef SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS
#define SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS 20
# endif


static DeviceLocation_t s_trackedDevices[SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS];
static uint16_t s_deviceCount;

static void
s_updateLocation(DDS_DataReader reader);

rc_t
BuiltInLocationUpdateService_init() {
    DDS_ReturnCode_t dds_ret;
    struct DDS_DataReaderListener locationListStruct = {
            .on_data_available = &s_updateLocation
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
    assert(devLoc);

    for (int i = 0; i < s_deviceCount; i++) {
        if (s_trackedDevices[i].device == device) {
            memcpy(devLoc, &s_trackedDevices[i], sizeof(DeviceLocation_t));
            return SDDS_RT_OK;
        }
    }

    return SDDS_RT_FAIL;
}

rc_t
BuiltInLocationUpdateService_getLocations(DeviceLocation_t** devices, uint16_t* size) {
    assert(devices);
    assert(size);

    *devices = s_trackedDevices;
    *size = s_deviceCount;
    return SDDS_RT_OK;
}

static void
s_updateLocation(DDS_DataReader reader) {
    assert(reader);

    SDDS_DCPSLocation loc_data_used;
    SDDS_DCPSLocation* loc_data_used_ptr = &loc_data_used;
    DDS_ReturnCode_t dds_ret;
    rc_t ret;

    do {
        dds_ret = DDS_DCPSLocationDataReader_take_next_sample(reader, (SDDS_DCPSLocation**) &loc_data_used_ptr, NULL);
        if (dds_ret != DDS_RETCODE_NO_DATA) {
            int devPos = s_deviceCount;
            for (int i = 0; i < s_deviceCount; i++) {
                if (s_trackedDevices[i].device == loc_data_used.device) {
                    devPos = i;
                    break;
                }
            }

            if (devPos < SDDS_BUILTIN_LOCATION_UPDATE_MAX_DEVS) {
                Geometry_initEllipse(&s_trackedDevices[devPos].area);
                s_trackedDevices[devPos].device = loc_data_used.device;
                s_trackedDevices[devPos].area.basicShape.geometry.type = GEO_TYPE_ELLIPSE;
                s_trackedDevices[devPos].area.basicShape.vertex.geometry.type = GEO_TYPE_POINT;
                s_trackedDevices[devPos].area.basicShape.vertex.x = loc_data_used.x;
                s_trackedDevices[devPos].area.basicShape.vertex.y = loc_data_used.y;
                s_trackedDevices[devPos].area.basicShape.vertex.z = loc_data_used.z;
                s_trackedDevices[devPos].area.basicShape.length = loc_data_used.length;
                s_trackedDevices[devPos].area.basicShape.width = loc_data_used.width;
                s_trackedDevices[devPos].expiration = loc_data_used.expiration;
                Time_getTime16(&s_trackedDevices[devPos].time);
                s_trackedDevices[devPos].time += loc_data_used.age;

                if (devPos == s_deviceCount) {
                    s_deviceCount++;
                }
#ifdef SDDS_SUBSCRIPTION_MANAGER
                ret = SubscriptionManagementService_evalFilteredSubscription(&s_trackedDevices[devPos]);
                if (ret != SDDS_RT_OK) {
                    Log_error("SubscriptionManagementService_evalFilteredSubscription failed.\n");
                }
#endif
            }
            else {
                Log_error("Maximum of available devices reached.\n");
            }
        }
    } while ((dds_ret != DDS_RETCODE_NO_DATA) && (dds_ret != DDS_RETCODE_ERROR));
//    DeviceLocation_t* devices;
//    uint16_t size;
//    ret = BuiltInLocationUpdateService_getLocations(&devices, &size);
//    if (ret == SDDS_RT_OK) {
//        printf("============= BuiltIn tracked devices =============\n");
//        for (int i = 0; i < size; i++) {
//            printf("DeviceLocation {\n"
//                   "\t device: %x\n"
//                   "\t area: (%d, %d, %d)\n"
//                   "\t time: %d\n"
//                   "\t expiration: %u\n"
//                   "}\n", devices[i].device,
//                          devices[i].area.basicShape.vertex.x,
//                          devices[i].area.basicShape.vertex.y,
//                          devices[i].area.basicShape.vertex.z,
//                          devices[i].time,
//                          devices[i].expiration);
//        }
//        printf("====================================================\n");
//    }
//    else {
//        Log_error("BuiltInLocationUpdateService_getLocations failed.\n");
//    }
}

#endif
