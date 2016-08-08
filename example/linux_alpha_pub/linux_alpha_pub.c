#include <unistd.h>
#include "linux_alpha_pub_sdds_impl.h"

uint16_t g_LocationService_simulatedDevicesCount;
DeviceLocation_t* g_LocationService_simulatedDevices;

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(4);  // Disable logs, set to 0 for to see everything.

	g_LocationService_simulatedDevicesCount = 10;
	g_LocationService_simulatedDevices = malloc(g_LocationService_simulatedDevicesCount * sizeof(struct DeviceLocation));

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

    static Alpha alpha_pub;
    alpha_pub.value = 'H';
    strncpy (alpha_pub.value2,   "Es gibt im", 10);
    alpha_pub.value3 = 1;
    alpha_pub.device = NodeConfig_getNodeID(); 

    uint8_t sec = 0;

    for (;;) {
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);
        if (ret != DDS_RETCODE_OK) {
            printf ("Failed to send topic alpha\n");
        }
        else {
            printf ("Send a alpha sample\n");
        }

        sleep (1);
        sec++;
        sec = sec % 40;

        if (sec == 19) {
            g_LocationService_simulatedDevices[0].area.basicShape.vertex.x = 1000;
            g_LocationService_simulatedDevices[0].area.basicShape.vertex.y = 1000;
        }
        if (sec == 39) {
            g_LocationService_simulatedDevices[0].area.basicShape.vertex.x = 10;
            g_LocationService_simulatedDevices[0].area.basicShape.vertex.y = 3;
        }
    }

    return 0;
}
