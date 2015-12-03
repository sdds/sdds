#include "os-ssal/NodeConfig.h"

#include "gen_constants.h"

#include "cpuid.h"

#ifndef PLATFORM_RIOT_SDDS_IFACE
#define PLATFORM_RIOT_SDDS_IFACE 0
#endif

SSW_NodeID_t
NodeConfig_getNodeID(void) {
    SSW_NodeID_t nodeID = 0;
    uint32_t cpuid;
    cpuid_get(&cpuid);
    nodeID = cpuid & 0xFFFF;
    return nodeID;
}