#include <unistd.h>
#include "riot_location_tracking_sim_sdds_impl.h"

#define DEV_COUNT   1

uint16_t g_LocationService_simulatedDevicesCount = DEV_COUNT;
DeviceLocation_t* g_LocationService_simulatedDevices;

void move(SSW_NodeID_t dev, uint16_t x, uint16_t y, uint16_t z) {
        int i = 0;
        for (i = 0; i < g_LocationService_simulatedDevicesCount; i++) {
            if (g_LocationService_simulatedDevices[i].device == dev) {
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.x = x;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.y = y;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.z = z;
                g_LocationService_simulatedDevices[i].area.basicShape.width = 5;
                g_LocationService_simulatedDevices[i].area.basicShape.length = 5;
                g_LocationService_simulatedDevices[i].expiration = 0;
                Time_getTime16(&g_LocationService_simulatedDevices[i].time);
                break;
            }
        }

        if (i == g_LocationService_simulatedDevicesCount) {
            printf("\nDevice %x not found\n", dev);
            return;
        }

        printf("\nSet new location ((%u, %u, %u), 5, 5) for device %x\n", x, y, z, dev);
}

int main()
{

    uint16_t device[DEV_COUNT]; 
	/*
		Enter the publishers device ID here
	*/
    device[0] = 0x0000; // publisher (bathroom)


	DDS_ReturnCode_t ret;

	Log_setLvl(2);  // Disable logs, set to 0 to see everything.
	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	g_LocationService_simulatedDevicesCount = DEV_COUNT;
	g_LocationService_simulatedDevices = malloc(DEV_COUNT * sizeof(struct DeviceLocation));

    memset(g_LocationService_simulatedDevices, 0, DEV_COUNT * sizeof(struct DeviceLocation));

    for (int i = 0; i < DEV_COUNT; i++) {
        g_LocationService_simulatedDevices[i].device = device[i];
        Geometry_initEllipse(&g_LocationService_simulatedDevices[i].area);
        if (LocationTrackingService_trackDevice(device[i]) != SDDS_RT_OK) {
            printf("Unable to track device %x\n", device[i]);
            return 1;
        }
    }

    move(device[0], 500, 500, 0);   // bathroom
    for (;;) {
        sleep(30);
    }

    return 0;
}
