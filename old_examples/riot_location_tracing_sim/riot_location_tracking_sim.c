#include <unistd.h>
#include "riot_location_tracking_sim_sdds_impl.h"

uint16_t g_LocationService_simulatedDevicesCount;
DeviceLocation_t* g_LocationService_simulatedDevices;

int main()
{

    const uint16_t devCount = 2;
    uint16_t device[devCount]; 
    device[0] = 0x9826;
    device[1] = 0xd8ea;


	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(4);  // Disable logs, set to 0 for to see everything.

	g_LocationService_simulatedDevicesCount = devCount;
	g_LocationService_simulatedDevices = malloc(devCount * sizeof(struct DeviceLocation));

    memset(g_LocationService_simulatedDevices, 0, devCount * sizeof(struct DeviceLocation));

    for (int i = 0; i < devCount; i++) {
        g_LocationService_simulatedDevices[i].device = device[i];
        Geometry_initEllipse(&g_LocationService_simulatedDevices[i].area);
        if (LocationTrackingService_trackDevice(device[i]) != SDDS_RT_OK) {
            printf("Unable to track device %x\n", device[i]);
            return 1;
        }
    }

    int locCase = 0;

    for (;;) {
        SSW_NodeID_t device;
        unsigned int x;
        unsigned int y;
        unsigned int z;
        unsigned int width;
        unsigned int length;
        unsigned int expiration;
        if (locCase == 1) {
            x = 100;
            y = 1100;
            z = 0;
            width = 5;
            length = 5;
            expiration = 0;
        }
        else if (locCase == 2) {
            x = 1000;
            y = 0;
            z = 0;
            width = 5;
            length = 5;
            expiration = 0;
        }
        else {
            x = 0;
            y = 0;
            z = 0;
            width = 5;
            length = 5;
            expiration = 0;
        }
        device = 0x9826;

        int i = 0;
        for (i = 0; i < g_LocationService_simulatedDevicesCount; i++) {
            if (g_LocationService_simulatedDevices[i].device == device) {
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.x = x;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.y = y;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.z = z;
                g_LocationService_simulatedDevices[i].area.basicShape.width = width;
                g_LocationService_simulatedDevices[i].area.basicShape.length = length;
                g_LocationService_simulatedDevices[i].expiration = expiration;
                Time_getTime16(&g_LocationService_simulatedDevices[i].time);
                break;
            }
        }

        if (i == g_LocationService_simulatedDevicesCount) {
            printf("\nDevice %x not found\n", device);
            continue;
        }

        printf("\nSet new location ((%u, %u, %u), %u, %u) for device %x\n", x, y, z, width, length, device);
        sleep(30);
        //locCase++;
        //locCase %= 3;
    }

    return 0;
}
