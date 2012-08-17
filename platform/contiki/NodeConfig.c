/*
 * NodeConfig.c
 *
 *  Created on: 12 Jun 2012
 *      Author: k_beckma
 */


#include "os-ssal/NodeConfig.h"
#include "Log.h"

#include <contiki.h>
#include <contiki-net.h>

SSW_NodeID_t NodeConfig_getNodeID(void) {

	SSW_NodeID_t id = 0;

    for (uint8_t i=0; i < UIP_DS6_ADDR_NB; i++) {
	if (uip_ds6_if.addr_list[i].isused) {
	    id = uip_ntohs(uip_ds6_if.addr_list[i].ipaddr.u16[7]);
 	    Log_debug("node ID: 0x%4x \n ", id);
	}
    }

    return id;

}
