#include "os-ssal/NodeConfig.h"

#include "gen_constants.h"
#include "sdds/Log.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#ifndef PLATFORM_LINUX_SDDS_IFACE
#define PLATFORM_LINUX_SDDS_IFACE "tap0"
#endif

SSW_NodeID_t NodeConfig_getNodeID(void) {
	SSW_NodeID_t nodeID = 0;
	unsigned char *mac;
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family != AF_INET
        &&  ifa->ifa_addr->sa_family != AF_INET6
        &&  strcmp (ifa->ifa_name, PLATFORM_LINUX_SDDS_IFACE) ==0) {
            //  The MAC address is from 10 to 15.
            Log_debug ("Obtained mac for %s: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
                    ifa->ifa_name,
                    ifa->ifa_addr->sa_data[10],
                    ifa->ifa_addr->sa_data[11],
                    ifa->ifa_addr->sa_data[12],
                    ifa->ifa_addr->sa_data[13],
                    ifa->ifa_addr->sa_data[14],
                    ifa->ifa_addr->sa_data[15]);
            mac = (unsigned char *) ifa->ifa_addr->sa_data + 10;
            nodeID = mac[5] | (mac[4] << 8);
            Log_debug ("NodeID is %"PRIx16"\n", nodeID);
        }
    }
    if (ifAddrStruct!=NULL)
        freeifaddrs(ifAddrStruct);

    return nodeID;
}
