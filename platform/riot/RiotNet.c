/*
 * =====================================================================================
 *
 *       Filename:  RiotNet.c
 *
 *    Description:  Impl for of the network interface for RIOT-OS
 *
 *        Version:  1.0
 *        Created:  29.02.2016 14:27:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "sdds/Network.h"
#include "sdds/sdds_types.h"
#include "RiotLocator.h"
#include "sdds/Log.h"

#include "local_constants.h"

#include "RiotNetHelper.h"

// RIOT imports
#include "net/af.h"
#include "net/conn/ip.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include <net/gnrc/netif.h>
#include <net/conn/udp.h>
#include <msg.h>
#include <ps.h>
#include <sema.h>

//////////////////////// DEFINES //////////////////////
#define MAIN_QUEUE_SIZE     (8)



///////////////////// local methodes ///////////////////

/**
 * Initialises the multicast part for buildintopics
 */
rc_t
_initMulticast(void);

rc_t
_initUnicast(void);

rc_t
_joinMulticastGroup(char* group);

void*
_receiverThreadFunction(void* arg);





// temporäres define für ULA Adresse
#define SDDS_APP_ULA PLATFORM_RIOT_SDDS_IP_ADDRESS"/64"
//"fd29:144d:4196:94::123/64"

struct Network_t {
	// network interface of the LUA address
    // important it have to be the first element, otherwise it breaks NodeCongig
	gnrc_ipv6_netif_t* netif;


	ipv6_addr_t lua;
	ipv6_addr_t ula;

	conn_udp_t uni_con;
    kernel_pid_t uniReceiveThreadPid;

    sema_t initThreadSema;

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
	conn_udp_t multi_con;
    kernel_pid_t multiReceiveThreadPid;

#endif

};

struct NetworkThreadParameter {
    NetBuffRef_t* buff;
    conn_udp_t* conn;
    char typ[6];
    msg_t* mq;
    uint16_t port;
};

//////////////////////////// local storage ////////////////////////
// message queue
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static msg_t _unicast_msg_queue[MAIN_QUEUE_SIZE];

static char stackReceiveThreadUnicast[THREAD_STACKSIZE_DEFAULT];

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
static msg_t _multicast_msg_queue[MAIN_QUEUE_SIZE];
static char stackReceiveThreadMulticast[THREAD_STACKSIZE_DEFAULT];
#endif

// static datastructures
struct Network_t net = {.lua = IPV6_ADDR_UNSPECIFIED, .ula = IPV6_ADDR_UNSPECIFIED };
static NetBuffRef_t uniInBuff;
static NetBuffRef_t multiInBuff;


// function for receiver thread

void*
_receiverThreadFunction(void* arg) {


    // init
    struct NetworkThreadParameter* para = (struct NetworkThreadParameter*) arg;
    NetBuffRef_t* buff = para->buff;
    conn_udp_t* conn = para->conn;

    msg_init_queue(para->mq, MAIN_QUEUE_SIZE);

    ipv6_addr_t addrListen = IPV6_ADDR_UNSPECIFIED;

    // create connection
    int ret = conn_udp_create(conn, &addrListen, sizeof(ipv6_addr_t), AF_INET6, para->port);

    if (ret < 0) {
        Log_error("Can't create udp connection error code %d\n", ret);
        return NULL;
    }


// wake caller thread
    sema_post(&net.initThreadSema);

    // endless loop
    while (true) {
        Log_debug("Receiver loop - %s\n", para->typ);
        // renew in buffer
        NetBuffRef_renew(buff);

        // addr from sender
        ipv6_addr_t sourceAddr;
        uint16_t sourcePort;
        size_t addr_len = sizeof(sourceAddr);
        // receive blocking
        int ret = conn_udp_recvfrom(conn,
                                    buff->buff_start,
                                    buff->frame_start->size,
                                    &sourceAddr,
                                    &addr_len,
                                    &sourcePort);
        if (ret == 0) {
            // Everything is OK, but there was no data
            Log_debug("No Data");
            continue;
        } else if (ret < 0) {
            // error
            Log_error("Can't receive data from UDP connection. Error code: %d\n", ret);
            continue;
        }

        char addrStr[IPV6_ADDR_MAX_STR_LEN];
        Log_debug("Received %d bytes from %s port: %d \n", ret, ipv6_addr_to_str(addrStr, &sourceAddr, IPV6_ADDR_MAX_STR_LEN ), sourcePort);

        // get the source addr into a Locator
        RiotLocator_t sourceLoc;
        memcpy(&(sourceLoc.addr), &sourceAddr, sizeof(sourceAddr));

        // check if the locator is allready known
        Locator_t* loc;
        if (LocatorDB_findLocator((Locator_t*)&sourceLoc, &loc) != SDDS_RT_OK) {
            // not found we need a new one
            if (LocatorDB_newLocator(&loc) != SDDS_RT_OK) {
                continue;
            }
            RiotLocator_setIPv6Address((RiotLocator_t*) loc, &sourceAddr);
            ((RiotLocator_t*) loc)-> port = sourcePort;
        }

        // up ref counter
        Locator_upRef(loc);

        // set flags
        loc->isEmpty = false;
        loc->isSender = true;

        rc_t rc = buff->locators->add_fn(buff->locators, loc);
        if (rc != SDDS_RT_OK) {
            NetBuffRef_renew(buff);
            continue;
        }

        DataSink_processFrame(buff);
        NetBuffRef_renew(buff);
    }
}


rc_t
_initUnicast(void) {
	rc_t rc;
	// register ULA address
    // TODO

    char ULA_addr[IPV6_ADDR_MAX_STR_LEN];
    strcpy(ULA_addr, SDDS_APP_ULA);

    uint8_t prefix_len = ipv6_addr_split_prefix(ULA_addr);

	if (ipv6_addr_from_str(&net.ula, ULA_addr) != NULL) {
		// ok register ULA
		uint8_t flags =  GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST | GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO;

        char foo[IPV6_ADDR_MAX_STR_LEN];
        Log_debug("ULA, Str %s, len %d, converted %s\n ", ULA_addr, prefix_len, ipv6_addr_to_str(foo, &net.ula, IPV6_ADDR_MAX_STR_LEN ));
		ipv6_addr_t* res = gnrc_ipv6_netif_add_addr(net.netif->pid, &net.ula, prefix_len, flags);
		if (res == NULL) {
			Log_error("Could not add ULA Address %s to interface\n", SDDS_APP_ULA);
		} else {
            Log_debug("success: added %s/%d to interface %" PRIkernel_pid "\n", ULA_addr,
               prefix_len, net.netif->pid);
        }


	}
	// create unicast server socket
	Log_debug("Create UDP connection for port %d \n", TRANSPORT_IPV6_SDDS_PORT);

//    uint8_t laddr[16] = { 0 };
	//rc = _createSocket(&net.uni_con, TRANSPORT_IPV6_SDDS_PORT, &net.lua);
    /*rc = _createSocket(&net.uni_con, TRANSPORT_IPV6_SDDS_PORT, laddr);
	if (rc != SDDS_RT_OK) {
		return rc;
	}*/

    NetBuffRef_init(&uniInBuff);


	// parameter for receive thread
    static struct NetworkThreadParameter para = {.buff = &uniInBuff,
        .conn = &net.uni_con, .typ = "Uni", .mq = _unicast_msg_queue,
        .port = TRANSPORT_IPV6_SDDS_PORT};

    // create receive thread
    net.uniReceiveThreadPid = thread_create(stackReceiveThreadUnicast,
                                            sizeof(stackReceiveThreadUnicast),
                                            THREAD_PRIORITY_MAIN,
                                            THREAD_CREATE_STACKTEST, //  flags
                                            _receiverThreadFunction,
                                            (void*) &para,
                                            "Unicast receiver thread"
                                        );

    if (net.uniReceiveThreadPid > 0) {
        // OK
        return SDDS_RT_OK;
    } else if (net.uniReceiveThreadPid == -EINVAL) {
        Log_error("Priority of unicast receiver thread is to high\n");
        return SDDS_RT_FAIL;
    } else if (net.uniReceiveThreadPid == -EOVERFLOW) {
        Log_error("There are allready to many running threads. Can't start unicast receiver thread\n");
        return SDDS_RT_FAIL;
    } else {
        Log_error("Can't start unicast receiver thread. Error code %d\n", net.uniReceiveThreadPid);
    }



    return SDDS_RT_FAIL;

}

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
rc_t
_initMulticast(void)  {
	rc_t rc;
	// create multicast server socket


	// register mulicast addresses
	rc = _joinMulticastGroup(SDDS_BUILTIN_MULTICAST_ADDRESS);
    if (rc != SDDS_RT_OK) {
        Log_error("Can't register SDDS_BUILTIN_MULTICAST_ADDRESS %s\n", SDDS_BUILTIN_MULTICAST_ADDRESS);
        return rc;
    }
    Log_debug("Registerd multcast address %s\n", SDDS_BUILTIN_MULTICAST_ADDRESS);

	rc = _joinMulticastGroup(SDDS_BUILTIN_PARTICIPANT_ADDRESS);
    if (rc != SDDS_RT_OK) {
        Log_error("Can't register SDDS_BUILTIN_PARTICIPANT_ADDRESS %s\n", SDDS_BUILTIN_PARTICIPANT_ADDRESS);
        return rc;
    }
    Log_debug("Registerd multcast address %s\n", SDDS_BUILTIN_PARTICIPANT_ADDRESS);

	rc = _joinMulticastGroup(SDDS_BUILTIN_TOPIC_ADDRESS);
    if (rc != SDDS_RT_OK) {
        Log_error("Can't register SDDS_BUILTIN_TOPIC_ADDRESS %s\n", SDDS_BUILTIN_TOPIC_ADDRESS);
        return rc;
    }
    Log_debug("Registerd multcast address %s\n", SDDS_BUILTIN_TOPIC_ADDRESS);

	rc = _joinMulticastGroup(SDDS_BUILTIN_SUB_PUB_ADDRESS);
    if (rc != SDDS_RT_OK) {
        Log_error("Can't register SDDS_BUILTIN_SUB_PUB_ADDRESS %s\n", SDDS_BUILTIN_SUB_PUB_ADDRESS);
        return rc;
    }
    Log_debug("Registerd multcast address %s\n", SDDS_BUILTIN_SUB_PUB_ADDRESS);


    NetBuffRef_init(&multiInBuff);
	// create receive thread

    // parameter for receive thread
    static struct NetworkThreadParameter para = {.buff = &multiInBuff,
        .conn = &net.multi_con, .typ = "multi", .mq = _multicast_msg_queue,
        .port = (TRANSPORT_IPV6_SDDS_PORT + TRANSPORT_IPV6_SDDS_BUILTIN_MULTICAST_PORT_OFF)};

    // create receive thread
    net.multiReceiveThreadPid = thread_create(stackReceiveThreadMulticast,
                                            sizeof(stackReceiveThreadMulticast),
                                            THREAD_PRIORITY_MAIN,
                                            THREAD_CREATE_STACKTEST, //  flags
                                            _receiverThreadFunction,
                                            (void*) &para,
                                            "Multicast receiver thread"
                                        );

    if (net.multiReceiveThreadPid > 0) {
        // OK
        return SDDS_RT_OK;
    } else if (net.multiReceiveThreadPid == -EINVAL) {
        Log_error("Priority of Multicast receiver thread is to high\n");
        return SDDS_RT_FAIL;
    } else if (net.multiReceiveThreadPid == -EOVERFLOW) {
        Log_error("There are allready to many running threads. Can't start Multicast receiver thread\n");
        return SDDS_RT_FAIL;
    } else {
        Log_error("Can't start Multicast receiver thread. Error code %d\n", net.multiReceiveThreadPid);
    }

    return SDDS_RT_FAIL;
}

rc_t
_joinMulticastGroup(char* group) {

	ipv6_addr_t addr;
	if (ipv6_addr_from_str(&addr, group) == NULL) {
		Log_error("IPv6 multicast address %s is not a valid IPv6 address\n", group);
		return SDDS_RT_BAD_PARAMETER;
	}
	if (ipv6_addr_is_multicast(&addr) == false) {
		Log_error("IPv6 address %s is not a multicast address\n", group);
		return SDDS_RT_BAD_PARAMETER;
	}

	uint8_t prefixLen = RiotNetHelper_get_prefix_len(group);

	if (gnrc_ipv6_netif_add_addr(net.netif->pid, &addr, prefixLen, GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO) == NULL) {
		Log_error("Can't register multicast address %s\n", group);
		return SDDS_RT_FAIL;
	}

	return SDDS_RT_OK;
}
#endif // FEATURE_SDDS_MULTICAST_ENABLED


rc_t
Network_init(void) {

	rc_t rc;

	// set net structure
	memset(&net, 0, sizeof(struct Network_t));

	// init riot msg queue
	msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

	kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t numof = gnrc_netif_get(ifs);

        for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
            RiotNetHelper_netif_list(ifs[i]);
        }

	// get link local unicast address
//	kernel_pid_t ifs[GNRC_NETIF_NUMOF];
//	size_t numof = gnrc_netif_get(ifs);
	if (numof > 0) {
        gnrc_ipv6_netif_t* entry = gnrc_ipv6_netif_get(ifs[0]);
        for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
        	if ((ipv6_addr_is_link_local(&entry->addrs[i].addr)) && !(entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST)) {
				// store netif pointer
				net.netif = entry;
				memcpy(&net.lua, &entry->addrs[i].addr, sizeof(ipv6_addr_t));
				break;
			}
        }
    }


	Log_debug("Iface %3d net %3d \n", ifs[0], net.netif->pid);

	// set channel
	RiotNetHelper_set_channel(net.netif, SDDS_6LOWPAN_CHANNEL);
	Log_debug("6LowPAN channel: %" PRIu16 " \n", RiotNetHelper_get_channel(net.netif));

	uint16_t panId;
	RiotNetHelper_get_nid(net.netif, &panId);
	Log_debug("6LowPAN pan id: 0x%x\n", panId);


    char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
    ipv6_addr_to_str(ipv6_addr, &net.lua, IPV6_ADDR_MAX_STR_LEN);
    printf("My LUA address is %s\n", ipv6_addr);

    // set up mutex so threads create connection first
    sema_create(&net.initThreadSema, 0);

	rc = _initUnicast();
	if (rc != SDDS_RT_OK) {
		return rc;
	}
    // second lock, wait till thread is ready
    sema_wait(&net.initThreadSema);

#ifdef FEATURE_SDDS_MULTICAST_ENABLED

	rc = _initMulticast();
	if (rc != SDDS_RT_OK) {
		return rc;
	}
    sema_wait(&net.initThreadSema);
#endif

    Log_debug("foo\n");
    ps();

    Log_debug("bar\n");

//	RiotNetHelper_printAllAddresses();
/*
    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
        RiotNetHelper_netif_list(ifs[i]);
    }
*/

    sema_destroy(&net.initThreadSema);

	return SDDS_RT_OK;
}

size_t
Network_size(void) {
	return sizeof(struct Network_t);
}

rc_t
Network_send(NetBuffRef_t* buff) {

	assert(buff != NULL);

	// get head of locator list
	Locator_t* loc = buff->locators->first_fn(buff->locators);

	// iterate over list
	while (loc != NULL) {
		// get RiotLocator
		RiotLocator_t* dest = (RiotLocator_t*) loc;

		// find the right interface for the address
		ipv6_addr_t src_addr;
		uint16_t sport;
		int src_addr_len;

		if (loc->type == SDDS_LOCATOR_TYPE_MULTI) {
#ifdef FEATURE_SDDS_MULTICAST_ENABLED
			src_addr_len = conn_udp_getlocaladdr(&net.multi_con, &src_addr, &sport);
			if (src_addr_len < 0) {
				// Error
				Log_error("Cant get local source address from UPD connection\n");
				return SDDS_RT_FAIL;
			}
#else
			Log_error("No Multicast configured");
            loc = buff->locators->next_fn(buff->locators);
			continue;
#endif

		} else if (loc->type == SDDS_LOCATOR_TYPE_UNI) {
			src_addr_len = conn_udp_getlocaladdr(&net.uni_con, &src_addr, &sport);
			if (src_addr_len < 0) {
				// Error
				Log_error("Cant get local source address from UPD connection\n");
				return SDDS_RT_FAIL;
			}
		} else {
			Log_error("Unknown Locator type");
            loc = buff->locators->next_fn(buff->locators);
			continue;
		}

		// send message
		int ret = conn_udp_sendto(buff->buff_start,
				buff->curPos, &src_addr, src_addr_len, &dest->addr,
				sizeof(ipv6_addr_t), AF_INET6, sport, dest->port);

        if (ret < 0) {
            Log_error("Can't send message");
        }

		loc = buff->locators->next_fn(buff->locators);
	}

	return SDDS_RT_OK;
}

void
Network_recvFrameHandler(Network _this);

rc_t
Network_getFrameBuff(NetFrameBuff* buff) {
	size_t size;

    size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
    size += sizeof(struct NetFrameBuff_t);

    *buff = Memory_alloc(size);

    if (*buff == NULL) {
        Log_error("out of memory\n");
        return SDDS_RT_NOMEM;
    }

    memset(*buff, 0, size);
    (*buff)->size = SDDS_NET_MAX_BUF_SIZE;

	return SDDS_RT_OK;
}

rc_t
Network_getPayloadBegin(size_t* startByte) {
	*startByte = sizeof(struct NetFrameBuff_t);
	return SDDS_RT_OK;
}
