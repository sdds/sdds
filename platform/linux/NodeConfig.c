#include "os-ssal/NodeConfig.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#ifndef PLATFORM_LINUX_SDDS_IFACE
#define PLATFORM_LINUX_SDDS_IFACE "tap0"
#endif

SSW_NodeID_t NodeConfig_getNodeID(void) {
	SSW_NodeID_t nodeID = 0;
	int fd;
	struct ifreq ifr;
	unsigned char *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, PLATFORM_LINUX_SDDS_IFACE, IFNAMSIZ - 1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);

	mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
	nodeID = mac[5] | (mac[4] << 8);

	return nodeID;
}
