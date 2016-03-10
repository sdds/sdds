/*
 * =====================================================================================
 *
 *       Filename:  RiotLocator.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  29.02.2016 15:14:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "sdds/Locator.h"
// RIOT Network
#include <net/ipv6/addr.h>
#include <byteorder.h>

struct RiotLocator_str
{
    Locator_t loc;
	ipv6_addr_t addr;
	uint16_t port;
};

typedef struct RiotLocator_str RiotLocator_t;


rc_t
RiotLocator_setIPv6Address(RiotLocator_t* loc, ipv6_addr_t* addr);
