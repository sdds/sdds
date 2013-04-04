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
 	    Log_debug("node native ID: 0x%4x \n ", id);
	}
    }
#ifdef CONTIKI_TARGET_AVR_ATMEGA128RFA1

    id = (id & 0x0fff) | 0x1000;
    Log_debug("WieDAS node ID: 0x%4x \n ", id);
#endif

#ifdef CONTIKI_TARGET_HEXABUS_SOCKET
    id = (id & 0x0fff) | 0x2000;
      Log_debug("WieDAS node ID: 0x%4x \n ", id);
#endif

    return id;

}
