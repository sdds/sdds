/*
 * =====================================================================================
 *
 *       Filename:  RiotLocator.c
 *
 *    Description:  Network interface for RIOT Locator-related
 *
 *        Version:  1.0
 *        Created:  29.02.2016 14:28:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include <string.h>
#include <byteorder.h>

#include "sdds/Network.h"
#include <sdds/sdds_types.h>
#include "os-ssal/Memory.h"
#include "sdds/Log.h"
#include "RiotLocator.h"

#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PORT_OFF                        20

rc_t
Network_createLocator(Locator_t** loc) {
	RiotLocator_t* newLoc = Memory_alloc(sizeof(RiotLocator_t));
	if (newLoc == NULL) {
		Log_error("Out of memory\n");
		return SDDS_RT_NOMEM;
	}
	*loc = (Locator_t*) newLoc;
	// set type for recvLoop
    (*loc)->type = SDDS_LOCATOR_TYPE_UNI;

    return Network_setAddressToLocator(*loc, "::1");
}

rc_t
Network_createMulticastLocator(Locator_t** loc) {
	RiotLocator_t* newLoc = Memory_alloc(sizeof(RiotLocator_t));
	if (newLoc == NULL) {
		Log_error("Out of memory\n");
		return SDDS_RT_NOMEM;
	}
	*loc = (Locator_t*) newLoc;
	(*loc)->type = SDDS_LOCATOR_TYPE_MULTI;

	return Network_setMulticastAddressToLocator(*loc, "ff02::1");
}

rc_t
RiotLocator_setIPv6Address(RiotLocator_t* loc, ipv6_addr_t* addr) {

	assert(loc != NULL);
	assert(addr != NULL);

	memcpy(&loc->addr, addr, sizeof(addr));

	if (ipv6_addr_is_multicast(addr) == true) {
		loc->loc.type = SDDS_LOCATOR_TYPE_MULTI;
	} else {
		loc->loc.type = SDDS_LOCATOR_TYPE_UNI;
	}

	return SDDS_RT_OK;
}
rc_t
Network_setMulticastAddressToLocator(Locator_t* loc, char* addr) {
	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}
	RiotLocator_t* rl = (RiotLocator_t*) loc;
	
	if (ipv6_addr_from_str(&rl->addr, addr) == NULL) {
		Log_error("Can't parse IPv6 address: %s\n", addr);
		return SDDS_RT_BAD_PARAMETER;
	}	
	if (ipv6_addr_is_multicast(&rl->addr) == false) {
		Log_error("IPv6 address %s is not a multicast address\n", addr);
		return SDDS_RT_BAD_PARAMETER;
	}

	return SDDS_RT_OK;
}

size_t
Network_locSize(void) {
	return sizeof(RiotLocator_t);
}

bool_t
Locator_isEqual(Locator_t* l1, Locator_t* l2) {
	if (l1 == NULL || l2 == NULL) {
		return false;
	}

	RiotLocator_t* rl1 = (RiotLocator_t*) l1;
	RiotLocator_t* rl2 = (RiotLocator_t*) l2;


	return ipv6_addr_equal(&rl1->addr, &rl2->addr);
}
rc_t
Network_set_locator_endpoint(Locator_t* loc, char* addr, int port) {
	assert(false);

	return SDDS_RT_OK;
}

rc_t
Network_setAddressToLocator(Locator_t* loc, char* addr) {
	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}
	RiotLocator_t* rl = (RiotLocator_t*) loc;
	
	if (ipv6_addr_from_str(&rl->addr, addr) == NULL) {
		Log_error("Can't parse IPv6 address: %s\n", addr);
		return SDDS_RT_BAD_PARAMETER;
	}	

	if (ipv6_addr_is_multicast(&rl->addr) == false) {
		rl->port = (PLATFORM_RIOT_SDDS_PORT);
	} else {
		rl->port =  (PLATFORM_RIOT_SDDS_PORT + PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PORT_OFF); 
	}

	return SDDS_RT_OK;
}

rc_t
Network_getSrcAddr(char* addr, int addr_len, char* port,
                   int port_len) {

	assert(false);
	return SDDS_RT_OK;

}

rc_t
Network_setPlatformAddressToLocator(Locator_t* loc) {
	return Network_setAddressToLocator(loc, PLATFORM_RIOT_SDDS_ADDRESS);
}

