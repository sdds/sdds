/*
 * NodeConfig.c
 *
 *  Created on: 12 Jun 2012
 *      Author: k_beckma
 */

#include "os-ssal/NodeConfig.h"
#include "Log.h"
#include "gen_constants.h"


SSW_NodeID_t NodeConfig_getNodeID(void) {
	SSW_NodeID_t nodeID = 0;

	nodeID = HW_ADDR5 | (HW_ADDR4 << 8);
	return nodeID;
}
