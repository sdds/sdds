/*
* =====================================================================================
*
*       Filename:  RiotNetHelper.h
*
*    Description:  Helper functions for the RIOT network module
*
*        Version:  1.0
*        Created:  05.03.2016 16:17:15
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
*        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
*
* =====================================================================================
*/

#ifndef _RIOTNETHELPER_H_
#define _RIOTNETHELPER_H_

#include <net/netopt.h>
#include <kernel_types.h>
#include <net/gnrc/ipv6.h>

#include "sdds/sdds_types.h"

uint8_t
RiotNetHelper_get_prefix_len(char *addr);

uint16_t
RiotNetHelper_get_channel(gnrc_ipv6_netif_t* netif);

rc_t
RiotNetHelper_get_nid(gnrc_ipv6_netif_t* netif, uint16_t* nid);

rc_t
RiotNetHelper_set_nid(gnrc_ipv6_netif_t* netif, uint16_t nid);

rc_t
RiotNetHelper_set_channel(gnrc_ipv6_netif_t* netif, uint16_t channel) ;

rc_t
RiotNetHelper_set_tx_power(gnrc_ipv6_netif_t* netif, int16_t tx);

rc_t
RiotNetHelper_get_tx_power(gnrc_ipv6_netif_t* netif, int16_t* tx);

void
RiotNetHelper_printAllAddresses(void);

void
RiotNetHelper_print_netopt_state(netopt_state_t state);

void
RiotNetHelper_netif_list(kernel_pid_t dev);

#endif
