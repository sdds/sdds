#include <unistd.h>
#include "linux_location_tracking_sim_sdds_impl.h"

uint16_t g_LocationService_simulatedDevicesCount;
DeviceLocation_t* g_LocationService_simulatedDevices;

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);  // Disable logs, set to 0 for to see everything.

	g_LocationService_simulatedDevicesCount = 10;
	g_LocationService_simulatedDevices = malloc(g_LocationService_simulatedDevicesCount * sizeof(struct DeviceLocation));

    memset(g_LocationService_simulatedDevices, 0, g_LocationService_simulatedDevicesCount * sizeof(struct DeviceLocation));

    g_LocationService_simulatedDevices[0].device = 0x9af6;
    g_LocationService_simulatedDevices[0].area.basicShape.geometry.type = GEO_TYPE_ELLIPSE;
    g_LocationService_simulatedDevices[0].area.basicShape.vertex.geometry.type = GEO_TYPE_POINT;
    g_LocationService_simulatedDevices[0].area.basicShape.vertex.x = 10;
    g_LocationService_simulatedDevices[0].area.basicShape.vertex.y = 3;
    g_LocationService_simulatedDevices[0].area.basicShape.vertex.z = 0;
    g_LocationService_simulatedDevices[0].area.basicShape.length = 5;
    g_LocationService_simulatedDevices[0].area.basicShape.width = 5;
    g_LocationService_simulatedDevices[0].expiration = 10000;
    Time_getTime16(&g_LocationService_simulatedDevices[0].time);

    g_LocationService_simulatedDevices[1].device = 0xd26a;
    g_LocationService_simulatedDevices[1].area.basicShape.geometry.type = GEO_TYPE_ELLIPSE;
    g_LocationService_simulatedDevices[1].area.basicShape.vertex.geometry.type = GEO_TYPE_POINT;
    g_LocationService_simulatedDevices[1].area.basicShape.vertex.x = 7;
    g_LocationService_simulatedDevices[1].area.basicShape.vertex.y = 25;
    g_LocationService_simulatedDevices[1].area.basicShape.vertex.z = 0;
    g_LocationService_simulatedDevices[1].area.basicShape.length = 5;
    g_LocationService_simulatedDevices[1].area.basicShape.width = 5;
    g_LocationService_simulatedDevices[1].expiration = 10000;
    Time_getTime16(&g_LocationService_simulatedDevices[1].time);

    ssw_rc_t ssw_ret = LocationTrackingService_trackDevice(g_LocationService_simulatedDevices[0].device);
    ssw_ret = LocationTrackingService_trackDevice(g_LocationService_simulatedDevices[1].device);

    for (;;) {
        SSW_NodeID_t device;
        unsigned int x;
        unsigned int y;
        unsigned int z;
    
        printf("\n\nchoose device");
        for (int i = 0; i < g_LocationService_simulatedDevicesCount; i++) {
            if (g_LocationService_simulatedDevices[i].device != 0) {
                printf(" [%x]", g_LocationService_simulatedDevices[i].device);
            }
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
