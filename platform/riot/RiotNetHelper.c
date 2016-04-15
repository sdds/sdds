/*
* =====================================================================================
*
*       Filename:  RiotNetHelper.c
*
*    Description:  Helper functions for the RIOT network module
*
*        Version:  1.0
*        Created:  05.03.2016 16:16:55
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
*        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
*
* =====================================================================================
*/

#include "RiotNetHelper.h"
#include "sdds/Log.h"

#include <net/ipv6.h>
#include <net/gnrc.h>
#include <net/gnrc/ipv6.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/sixlowpan/netif.h>

#define MAX_ADDR_LEN            (8U)


// c&p from RIOT sc_netif.c
uint8_t
RiotNetHelper_get_prefix_len(char *addr) {
    int prefix_len = ipv6_addr_split(addr, '/', 64);

    if ((prefix_len < 1) || (prefix_len > IPV6_ADDR_BIT_LEN)) {
        prefix_len = 64;
    }

    return prefix_len;
}

rc_t
RiotNetHelper_set_tx_power(gnrc_ipv6_netif_t* netif, int16_t tx) {

    int ret = gnrc_netapi_set(netif->pid, NETOPT_TX_POWER, 0, &tx, sizeof(uint16_t));
    if (ret < 0) {
        Log_error("Can't set tx power to vaule %" PRIi16 " dBm - error code: 0x%x\n", tx, ret);
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;

}

rc_t
RiotNetHelper_get_tx_power(gnrc_ipv6_netif_t* netif, int16_t* tx) {
    int ret = gnrc_netapi_get(netif->pid, NETOPT_TX_POWER, 0, tx, sizeof(uint16_t));
    if (ret != 0) {
        return SDDS_RT_FAIL;
    } else {
        return SDDS_RT_OK;
    }
}


uint16_t
RiotNetHelper_get_channel(gnrc_ipv6_netif_t* netif) {
    uint16_t channel;
    gnrc_netapi_get(netif->pid, NETOPT_CHANNEL, 1, &channel, sizeof(channel));
    return channel;
}

rc_t
RiotNetHelper_get_nid(gnrc_ipv6_netif_t* netif, uint16_t* nid) {

    int ret = gnrc_netapi_get(netif->pid, NETOPT_NID, 0, nid, sizeof(uint16_t));
    if (ret != 0) {
        return SDDS_RT_FAIL;
    } else {
        return SDDS_RT_OK;
    }


}

rc_t
RiotNetHelper_set_nid(gnrc_ipv6_netif_t* netif, uint16_t nid) {

    int ret = gnrc_netapi_set(netif->pid, NETOPT_NID, 0, (uint16_t *)&nid, sizeof(uint16_t));
    if (ret < 0) {
        Log_error("Can't set nid/pan_id to %" PRIu16 " error code: 0x%x\n", nid, ret);
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

rc_t
RiotNetHelper_set_channel(gnrc_ipv6_netif_t* netif, uint16_t channel) {

    int ret = gnrc_netapi_set(netif->pid, NETOPT_CHANNEL, 0, (uint16_t *)&channel, sizeof(uint16_t));
    if (ret < 0) {
        Log_error("Can't set channel to %" PRIu16 " error code: %d\n", channel, ret);
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

void
RiotNetHelper_printAllAddresses(void) {
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);
    if (numof > 0) {
        gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(ifs[0]);
        for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
            //       if ((ipv6_addr_is_link_local(&entry->addrs[i].addr)) &&
            //       !(entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST))
            //       {
            char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
            ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, IPV6_ADDR_MAX_STR_LEN);
            printf("My address is %s\n", ipv6_addr);
            //     }
        }
    }
}
// c&p from RIOT sc_netif.c
void
RiotNetHelper_print_netopt_state(netopt_state_t state) {
    switch (state) {
        case NETOPT_STATE_OFF:
        printf("OFF");
        break;
        case NETOPT_STATE_SLEEP:
        printf("SLEEP");
        break;
        case NETOPT_STATE_IDLE:
        printf("IDLE");
        break;
        case NETOPT_STATE_RX:
        printf("RX");
        break;
        case NETOPT_STATE_TX:
        printf("TX");
        break;
        case NETOPT_STATE_RESET:
        printf("RESET");
        break;
        default:
        /* nothing to do then */
        break;
    }
}
// c&p from RIOT sc_netif.c
void
RiotNetHelper_netif_list(kernel_pid_t dev)
{
    uint8_t hwaddr[MAX_ADDR_LEN];
    uint16_t u16;
    int16_t i16;
    uint8_t u8;
    int res;
    netopt_state_t state;
    netopt_enable_t enable = NETOPT_DISABLE;
    bool linebreak = false;

    #ifdef MODULE_GNRC_IPV6_NETIF
    gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(dev);
    char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
    #endif


    printf("Iface %2d  ", dev);

    res = gnrc_netapi_get(dev, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf(" HWaddr: ");
        printf("%s", gnrc_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
        hwaddr, res));
        printf(" ");
    }

    res = gnrc_netapi_get(dev, NETOPT_CHANNEL, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" Channel: %" PRIu16 " ", u16);
    }

    res = gnrc_netapi_get(dev, NETOPT_CHANNEL_PAGE, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" Page: %" PRIu16 " ", u16);
    }

    res = gnrc_netapi_get(dev, NETOPT_NID, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" NID: 0x%" PRIx16, u16);
    }

    printf("\n           ");

    res = gnrc_netapi_get(dev, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf("Long HWaddr: ");
        printf("%s ", gnrc_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
        hwaddr, res));
        linebreak = true;
    }

    if (linebreak) {
        printf("\n          ");
    }

    res = gnrc_netapi_get(dev, NETOPT_TX_POWER, 0, &i16, sizeof(i16));

    if (res >= 0) {
        printf(" TX-Power: %" PRIi16 "dBm ", i16);
    }

    res = gnrc_netapi_get(dev, NETOPT_STATE, 0, &state, sizeof(state));

    if (res >= 0) {
        printf(" State: ");
        RiotNetHelper_print_netopt_state(state);
        printf(" ");
    }

    res = gnrc_netapi_get(dev, NETOPT_RETRANS, 0, &u8, sizeof(u8));

    if (res >= 0) {
        printf(" max. Retrans.: %u ", (unsigned)u8);
    }

    res = gnrc_netapi_get(dev, NETOPT_CSMA_RETRIES, 0, &u8, sizeof(u8));

    if (res >= 0) {
        res = gnrc_netapi_get(dev, NETOPT_CSMA, 0, &enable, sizeof(enable));
        if ((res >= 0) && (enable == NETOPT_ENABLE)) {
            printf(" CSMA Retries: %u ", (unsigned)u8);
        }
    }

    printf("\n           ");

    res = gnrc_netapi_get(dev, NETOPT_PROMISCUOUSMODE, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("PROMISC  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_AUTOACK, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("AUTOACK  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_PRELOADING, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("PRELOAD  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_RAWMODE, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("RAWMODE  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_CSMA, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("CSMA  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_AUTOCCA, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("AUTOCCA  ");
        linebreak = true;
    }

    #ifdef MODULE_GNRC_IPV6_NETIF
    if (entry != NULL) {
        printf("MTU:%" PRIu16 "  ", entry->mtu);
        printf("HL:%u  ", (unsigned)entry->cur_hl);
        if (entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) {
            printf("6LO  ");
        }
        if (entry->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER) {
            printf("RTR  ");
        }
        if (entry->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV) {
            printf("RTR_ADV  ");
        }
        linebreak = true;

    }
    #endif

    #if defined(MODULE_GNRC_SIXLOWPAN_NETIF) && defined(MODULE_GNRC_SIXLOWPAN_IPHC)
    gnrc_sixlowpan_netif_t *sixlo_entry = gnrc_sixlowpan_netif_get(dev);

    if ((sixlo_entry != NULL) && (sixlo_entry->iphc_enabled)) {
        printf("IPHC  ");
        linebreak = true;
    }
    #endif

    if (linebreak) {
        printf("\n           ");
    }

    res = gnrc_netapi_get(dev, NETOPT_SRC_LEN, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf("Source address length: %" PRIu16 "\n           ", u16);
    }

    #ifdef MODULE_GNRC_IPV6_NETIF
    if (entry == NULL) {
        puts("");
        return;
    }

    printf("Link type: %s", (entry->flags & GNRC_IPV6_NETIF_FLAGS_IS_WIRED) ?
    "wired" : "wireless");
    printf("\n           ");

    for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (!ipv6_addr_is_unspecified(&entry->addrs[i].addr)) {
            printf("inet6 addr: ");

            if (ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr,
                IPV6_ADDR_MAX_STR_LEN)) {
                    printf("%s/%u  scope: ", ipv6_addr, (unsigned)entry->addrs[i].prefix_len);

                    if ((ipv6_addr_is_link_local(&entry->addrs[i].addr))) {
                        printf("local");
                    }
                    else {
                        printf("global");
                    }

                    if (entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST) {
                        if (ipv6_addr_is_multicast(&entry->addrs[i].addr)) {
                            printf(" [multicast]");
                        }
                        else {
                            printf(" [anycast]");
                        }
                    }
                }
                else {
                    printf("error in conversion");
                }

                printf("\n           ");
            }
        }
        #endif

        puts("");
    }
