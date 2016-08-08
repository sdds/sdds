#include <unistd.h>
#include "linux_location_tracking_sim_sdds_impl.h"

uint16_t g_LocationService_simulatedDevicesCount;
DeviceLocation_t* g_LocationService_simulatedDevices;

int main(int argc, char** argv)
{
    if(argc == 1) {
        printf("usage: %s [deviceID1] [deviceID2] ...\n", argv[0]);
        return 1;
    }

    int devCount = argc - 1;

	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

	g_LocationService_simulatedDevicesCount = devCount;
	g_LocationService_simulatedDevices = malloc(devCount * sizeof(struct DeviceLocation));

    memset(g_LocationService_simulatedDevices, 0, devCount * sizeof(struct DeviceLocation));

    for (int i = 0; i < devCount; i++) {
        int device;
        sscanf(argv[i+1], "%x", &device); 

        g_LocationService_simulatedDevices[i].device = device;
        Geometry_initEllipse(&g_LocationService_simulatedDevices[i].area);
        if (LocationTrackingService_trackDevice(device) != SDDS_RT_OK) {
            printf("Unable to track device %x\n", device);
            return 1;
        }
    }


    for (;;) {
        SSW_NodeID_t device;
        unsigned int x;
        unsigned int y;
        unsigned int z;
    
        printf("\n\nchoose device");
        for (int i = 0; i < g_LocationService_simulatedDevicesCount; i++) {
            printf(" [%x]", g_LocationService_simulatedDevices[i].device);
        }
        printf(": ");
        scanf("%x", &device);

        printf("x: ");
        scanf("%u", &x);
        printf("y: ");
        scanf("%u", &y);
        printf("z: ");
        scanf("%u", &z);

        int i = 0;
        for (i = 0; i < g_LocationService_simulatedDevicesCount; i++) {
            if (g_LocationService_simulatedDevices[i].device == device) {
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.x = x;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.y = y;
                g_LocationService_simulatedDevices[i].area.basicShape.vertex.z = z;
                Time_getTime16(&g_LocationService_simulatedDevices[i].time);
                break;
            }
        }

        if (i == g_LocationService_simulatedDevicesCount) {
            printf("\nDevice %x not found\n", device);
            continue;
        }

        printf("\nSet new location (%u, %u, %u) for device %x\n", x, y, z, device);
    }

    return 0;
}
