/*
 * LocationTrackingService.c
 *
 *  Created on: Aug 4, 2016
 *      Author: o_dedi
 */

#include "os-ssal/LocationTrackingService.h"
#include "os-ssal/Task.h"
#include "sDDS.h"

#ifdef FEATURE_SDDS_LOCATION_TRACKING_ENABLED

#   ifndef SDDS_LOCATION_TRACKING_MAX_DEVS
#define SDDS_LOCATION_TRACKING_MAX_DEVS 20
#   endif

#   ifndef SDDS_LOCATION_TRACKING_UPDATE_SEC
#define SDDS_LOCATION_TRACKING_UPDATE_SEC 0
#   endif

#   ifndef SDDS_LOCATION_TRACKING_UPDATE_MSEC
#define SDDS_LOCATION_TRACKING_UPDATE_MSEC 10
#   endif

static DeviceLocation_t s_devicePool[SDDS_LOCATION_TRACKING_MAX_DEVS];
static uint8_t s_freeDevs;
static List_t* s_trackedDevices;
static Task s_updateTask;

static void
s_updateDevices(void* obj);

ssw_rc_t
LocationTrackingService_init() {
    memset(&s_devicePool, 0, SDDS_LOCATION_TRACKING_MAX_DEVS * sizeof(struct DeviceLocation));
    s_freeDevs = SDDS_LOCATION_TRACKING_MAX_DEVS;
    s_trackedDevices = List_initConcurrentLinkedList();
    s_updateTask = Task_create();

    ssw_rc_t ret = Task_init(s_updateTask, s_updateDevices, NULL);
    if (SDDS_SSW_RT_OK != ret) {
        return SDDS_SSW_RT_FAIL;
    }
    ret = Task_start(s_updateTask, SDDS_LOCATION_TRACKING_UPDATE_SEC, SDDS_LOCATION_TRACKING_UPDATE_MSEC, SDDS_SSW_TaskMode_repeat);
    if (SDDS_SSW_RT_OK != ret) {
        return SDDS_SSW_RT_FAIL;
    }

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
LocationTrackingService_trackDevice(SSW_NodeID_t device) {
    assert(device);

    DeviceLocation_t* dev = (DeviceLocation_t*) s_trackedDevices->first_fn(s_trackedDevices);

    while (NULL != dev) {
        if (dev->device == device) {
            return SDDS_SSW_RT_OK;
        }
        dev = (DeviceLocation_t*) s_trackedDevices->next_fn(s_trackedDevices);
    }

    if (0 == s_freeDevs) {
        return SDDS_SSW_RT_FAIL;
    }

    int freePos = -1;
    for (int i = 0; i < SDDS_LOCATION_TRACKING_MAX_DEVS; i++) {
        if (0 == s_devicePool[i].device) {
            freePos = i;
            break;
        }
    }

    if (-1 == freePos) {
        return SDDS_SSW_RT_FAIL;
    }

    s_devicePool[freePos].device = device;

    rc_t ret = s_trackedDevices->add_fn(s_trackedDevices, (void*) &s_devicePool[freePos]);
    if (SDDS_RT_FAIL == ret) {

        s_devicePool[freePos].device = 0;
        return SDDS_SSW_RT_FAIL;
    }

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
LocationTrackingService_untrackDevice(SSW_NodeID_t device) {
    assert(device);

    DeviceLocation_t* dev = (DeviceLocation_t*) s_trackedDevices->first_fn(s_trackedDevices);
    while (NULL != dev) {
        if (dev->device == device) {
            rc_t ret = s_trackedDevices->delete_fn(s_trackedDevices);
            if (SDDS_RT_OK == ret) {
                memset(dev, 0, sizeof(struct DeviceLocation));
                return SDDS_SSW_RT_OK;
            }
            return SDDS_SSW_RT_FAIL;
        }
        dev = (DeviceLocation_t*) s_trackedDevices->next_fn(s_trackedDevices);
    }
    return SDDS_SSW_RT_FAIL;
}

ssw_rc_t
LocationTrackingService_getDeviceLocation(SSW_NodeID_t device, DeviceLocation_t* devLoc) {
    assert(device);
    assert(devLoc);

    DeviceLocation_t* dev = (DeviceLocation_t*) s_trackedDevices->first_fn(s_trackedDevices);
    while (NULL != dev) {
        if (dev->device == device) {
            memcpy(devLoc, dev, sizeof(struct DeviceLocation));
            return SDDS_SSW_RT_OK;
        }
        dev = (DeviceLocation_t*) s_trackedDevices->next_fn(s_trackedDevices);
    }
    return SDDS_SSW_RT_FAIL;
}

List_t*
LocationTrackingService_getLocations() {
    return s_trackedDevices;
}

static void
s_updateDevices(void* obj) {
    DeviceLocation_t* dev = (DeviceLocation_t*) s_trackedDevices->first_fn(s_trackedDevices);
    while (NULL != dev) {
        ssw_rc_t ret = LocationService_getDeviceLocation(dev->device, dev);
        if (SDDS_SSW_RT_OK != ret) {
            Log_error("Obtaining location failed for device %x\n", dev->device);
        }
        dev = (DeviceLocation_t*) s_trackedDevices->next_fn(s_trackedDevices);
    }
}

#endif
