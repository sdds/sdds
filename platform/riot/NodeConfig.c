#include "os-ssal/NodeConfig.h"

#include "gen_constants.h"

#include <net/eui64.h>
#include <net/gnrc/netapi.h>
#include <net/ipv6.h>
#include <net/gnrc.h>
#include <net/gnrc/ipv6.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/sixlowpan/netif.h>

#include "sdds/Log.h"

 #include <string.h>

#ifndef PLATFORM_RIOT_SDDS_IFACE
#define PLATFORM_RIOT_SDDS_IFACE 0
#endif

struct Network_t {
	// network interface of the LUA address
    // important it have to be the first element, otherwise it breaks NodeCongig
	gnrc_ipv6_netif_t* netif;
};
extern struct Network_t net;

SSW_NodeID_t
NodeConfig_getNodeID(void) {
    SSW_NodeID_t nodeID = (uint16_t) 0xaabbccdd;

    eui64_t eui;
    memset(&eui, 0, sizeof(eui64_t));

    if (gnrc_netapi_get(net.netif->pid, NETOPT_IPV6_IID, 0, &eui, sizeof(eui64_t))  > 0) {
        nodeID = (((uint16_t)eui.uint8[6])<<8);
        nodeID = nodeID | ((uint16_t)eui.uint8[7]);
    }
/*
    Log_debug("EUI: %2x:%2x:%2x:%2x:%2x:%2x:%2x:%2x Node ID 0x%4x \n",
            eui.uint8[0],
            eui.uint8[1],
            eui.uint8[2],
            eui.uint8[3],
            eui.uint8[4],
            eui.uint8[5],
            eui.uint8[6],
            eui.uint8[7],
            nodeID);
            */
    
    return nodeID;
}
