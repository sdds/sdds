#include "os-ssal/NodeConfig.h"

#include "gen_constants.h"
#include <api_calls.h>
#include <Log.h>
#include "lwip/netif.h"

SSW_NodeID_t NodeConfig_getNodeID(void) {
	struct netif netif = *netif_default;
	SSW_NodeID_t id = ((netif.hwaddr[4] << 8)|(netif.hwaddr[5]));
	//Log_debug("ID: %d\n", id);
	return id;
}
