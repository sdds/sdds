#include "os-ssal/NodeConfig.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

SSW_NodeID_t NodeConfig_getNodeID(void) {
	SSW_NodeID_t nodeID = 0;

	struct ifreq ifr;
	struct ifconf ifc;
	char buf[1024];
	int success = 0;

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock == -1) {
		/* handle error*/
	};

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
		/* handle error */
	}

	struct ifreq* it = ifc.ifc_req;
	const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

	for (; it != end; ++it) {
		strcpy(ifr.ifr_name, it->ifr_name);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
			if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
				if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
					success = 1;
					break;
				}
			}
		} else { /* handle error */
		}
	}

	unsigned char mac_address[6];

	if (success) {
		memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
		nodeID = mac_address[5] | (mac_address[4] << 8);
	}

//	int i;
//	printf("MAC: ");
//	for (i = 0; i < 6; i++) {
//		printf("[%x]", mac_address[i]);
//	}
//	printf(" nodeID: %x\n", nodeID);

	return nodeID;
}