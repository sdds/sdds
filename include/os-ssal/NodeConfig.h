/*
 * NodeConfig.h
 *
 * Config-API for Nodes
 *
 *  Created on: 12 Jun 2012
 *      Author: k_beckma
 */

#ifndef NODECONFIG_H_
#define NODECONFIG_H_

#include "SSW.h"
#include "sdds/sdds_types.h"

typedef uint16_t SSW_NodeID_t;

SSW_NodeID_t
NodeConfig_getNodeID(void);


#endif /* NODECONFIG_H_ */