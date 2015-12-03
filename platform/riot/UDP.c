#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/Log.h>
#include <sdds/LocatorDB.h>
#include <sdds/NetBuffRef.h>
#include <sdds/sdds_types.h>
#include "gen_constants.h"

//#include "arpa/inet.h"

// RIOT
#include "thread.h"
#include "socket_base/socket.h"
#include "socket_base/in.h"
#include "socket_base/types.h"
#include "inet_pton.h"
#include "inet_ntop.h"
#include "net_if.h"
#include "rpl.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define PLATFORM_RIOT_IPV6_MAX_CHAR_LEN 45

#ifndef PLATFORM_RIOT_SDDS_PORT
#define PLATFORM_RIOT_SDDS_PORT 23234
#endif

#ifndef PLATFORM_RIOT_SDDS_PROTOCOL
// only use AF_INET6
#define PLATFORM_RIOT_SDDS_PROTOCOL AF_INET6
#endif

#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_ADDRESS                SDDS_BUILTIN_MULTICAST_ADDRESS
#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS    SDDS_BUILTIN_PARTICIPANT_ADDRESS
#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS          SDDS_BUILTIN_TOPIC_ADDRESS
#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS        SDDS_BUILTIN_SUB_PUB_ADDRESS

#define PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PORT_OFF 50


#ifndef PLATFORM_RIOT_SDDS_ADDRESS
#define PLATFORM_RIOT_SDDS_ADDRESS "::"
#endif

#define SDDS_IF 0

#ifdef MODULE_NATIVENET
#define PTON_AF_INET6 10
#else
#define PTON_AF_INET6 PLATFORM_RIOT_SDDS_PROTOCOL
#endif

struct Network_t {
    int fd_uni_socket;
    int fd_multi_socket;
    int port;
    int recvThread;
    int multiRecvThread;
};

//typedef struct Network_t Network;

struct RiotLocator_str
{
    Locator_t loc;
    sockaddr6_t addr_storage;
};

typedef struct RiotLocator_str RiotLocator_t;

static struct Network_t net;
static NetBuffRef_t inBuff;
static NetBuffRef_t multiInBuff;

const int recv_stack_size =  KERNEL_CONF_STACKSIZE_PRINTF_FLOAT * sizeof(char);

void*
recvLoop(void*);
static int
create_socket(int port);

#ifdef MODULE_NATIVENET
static void
sixlowapp_ndp_workaround(ipv6_addr_t* dest);
#endif

// for the builtintopic
// IF BUILTIN
Locator_t* builtinTopicNetAddress;
// ENDIF

rc_t
Network_Multicast_init(void);
rc_t
Network_createLocator(Locator_t** loc);
rc_t
Network_createMulticastLocator(Locator_t** loc);
rc_t
Network_Multicast_joinMulticastGroup(char* group);



size_t
Network_size(void) {
    return sizeof(struct Network_t);
}

rc_t
Network_init(void) {
    sixlowpan_lowpan_init_interface(SDDS_IF);

    // search in the db for the locator
    // TODO do something better than this hack here....
    net.fd_uni_socket = create_socket(PLATFORM_RIOT_SDDS_PORT);

    if (net.fd_uni_socket < 0) {
        return SDDS_RT_FAIL;
    }

    // init the incoming frame buffer and add dummy unicast locator
    NetBuffRef_init(&inBuff);
    int ret = Network_createLocator(&inBuff.addr);
    if(ret == SDDS_RT_FAIL) {
        return SDDS_RT_FAIL;
    }

    // set up a thread to read from the udp socket
    char* stack_thread = (char*) Memory_alloc(recv_stack_size);
    net.recvThread = thread_create(stack_thread, recv_stack_size,
                                   THREAD_PRIORITY_MAIN, 0,
                                   recvLoop, (void*)&inBuff, "recvThread sDDS");

    if (net.recvThread < 0) {
        exit(-1);
    }

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
    Network_Multicast_init();
#endif

    return SDDS_RT_OK;
}

rc_t
Network_Multicast_init() {
    rc_t ret;

    net.fd_multi_socket = create_socket(PLATFORM_RIOT_SDDS_PORT + PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PORT_OFF);

    ret = Network_Multicast_joinMulticastGroup(PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    ret = Network_Multicast_joinMulticastGroup(PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    ret = Network_Multicast_joinMulticastGroup(PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    ret = Network_Multicast_joinMulticastGroup(PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    NetBuffRef_init(&multiInBuff);
    Network_createMulticastLocator(&multiInBuff.addr);

    // set up a thread to read from the udp socket [multicast]
    char* stack_thread = (char*) Memory_alloc(recv_stack_size);
    net.recvThread = thread_create(stack_thread, recv_stack_size,
                                   THREAD_PRIORITY_MAIN, 0,
                                   recvLoop, (void*)&multiInBuff, "recvMultiThread sDDS");

    return SDDS_RT_OK;
}

rc_t
Network_Multicast_joinMulticastGroup(char* group) {
    ipv6_addr_t tmp;

    int ret = inet_pton(PTON_AF_INET6, group, &tmp);
    if(ret != 1) {
        return SDDS_RT_FAIL;
    }

    if (!ipv6_net_if_add_addr(SDDS_IF, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

static int
create_socket(int port) {
    int fd = socket_base_socket(PLATFORM_RIOT_SDDS_PROTOCOL, SOCK_DGRAM, 0);

    if (fd < 0) {
        return -1;
    }

#if PLATFORM_RIOT_SDDS_PROTOCOL != AF_INET6
#error "Only AF_INET6 are understood RIOT protocols."
#endif

    // bind the socket
    sockaddr6_t sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin6_family = PLATFORM_RIOT_SDDS_PROTOCOL;
    sa.sin6_port = HTONS(port);

    int ret = inet_pton(PTON_AF_INET6, PLATFORM_RIOT_SDDS_ADDRESS, &(sa.sin6_addr));
    if(ret != 1) {
        return -1;
    }
#ifdef MODULE_NATIVENET
    sixlowapp_ndp_workaround(&sa.sin6_addr);
#endif
    if (socket_base_bind(fd, &sa, sizeof(sa)) != 0) {
        return -1;
    }

    return fd;
}

void*
recvLoop(void* netBuff) {
    NetBuffRef_t* buff = (NetBuffRef_t*) netBuff;
    int sock; // receive socket
    unsigned int sock_type; // broad or multicast
    sockaddr6_t addr;


    // Check the dummy locator for uni or multicast socket
    Locator_t* l = (Locator_t*) buff->addr;
    sock_type = l->type;

    if(sock_type == SDDS_LOCATOR_TYPE_MULTI) {
        sock = net.fd_multi_socket;
    }
    else if(sock_type == SDDS_LOCATOR_TYPE_UNI) {
        sock = net.fd_uni_socket;
    }

    while (true)
    {
        //reset the buffer
        NetBuffRef_renew(buff);

        uint32_t addr_len = sizeof(sockaddr6_t);
        buff->addr = &addr;
        ssize_t recv_size = socket_base_recvfrom(sock, buff->buff_start,
                                                 buff->frame_start->size, 0,
                                                 (sockaddr6_t*)buff->addr, &addr_len);
        //(struct sockaddr *)&addr, &addr_len);

        if (recv_size == -1) {
            continue;
        }

        RiotLocator_t sloc;
        memcpy(&(sloc.addr_storage), &buff->addr, addr_len);

        Locator_t* loc;
        if (LocatorDB_findLocator((Locator_t*)&sloc, &loc) != SDDS_RT_OK) {
            // not found we need a new one
            if (LocatorDB_newLocator(&loc) != SDDS_RT_OK) {
                continue;
            }
            memcpy(&((RiotLocator_t*)loc)->addr_storage, &buff->addr, addr_len);
        }

        // up ref counter
        Locator_upRef(loc);

        loc->isEmpty = false;
        loc->isSender = true;
        loc->type = sock_type;


        if(sock_type == SDDS_LOCATOR_TYPE_MULTI) {
            multiInBuff.addr = loc;
        }
        else if(sock_type == SDDS_LOCATOR_TYPE_UNI) {
            inBuff.addr = loc;
        }

        DataSink_processFrame(buff);
        LocatorDB_freeLocator(loc);
    }
    return SDDS_RT_OK;
}

rc_t
Network_send(NetBuffRef_t* buff) {
    int sock;
    unsigned int sock_type;

    // Check the locator for uni or multicast socket
    Locator_t* l = (Locator_t*) buff->addr;
    sock_type = l->type;
    // add locator to the netbuffref
    if(sock_type == SDDS_LOCATOR_TYPE_MULTI) {
        sock = net.fd_multi_socket;
    }
    else if(sock_type == SDDS_LOCATOR_TYPE_UNI) {
        sock = net.fd_uni_socket;
    }

    Locator_t* loc = buff->addr;

    while (loc != NULL ) {
        ssize_t transmitted;
        sockaddr6_t* addr =
            (sockaddr6_t*) &((RiotLocator_t*) loc)->addr_storage;
        char addr_str[IPV6_MAX_ADDR_STR_LEN];
        inet_ntop(PLATFORM_RIOT_SDDS_PROTOCOL, &(addr->sin6_addr), addr_str, IPV6_MAX_ADDR_STR_LEN);
        transmitted = socket_base_sendto(sock, buff->buff_start, buff->curPos,
                                         0, addr, sizeof(sockaddr6_t));

        if (transmitted == -1) {
            Log_error("can't send udp packet\n");
            return SDDS_RT_FAIL;
        }
        loc = loc->next;
    }
    return SDDS_RT_OK;
}

void
Network_recvFrameHandler(Network _this) {
    (void)_this;
}


rc_t
Network_getFrameBuff(NetFrameBuff* buff) {
    size_t size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
    size += sizeof(struct NetFrameBuff_t);

    *buff = Memory_alloc(size);

    if (*buff == NULL) {
        return SDDS_RT_NOMEM;
    }
    memset(*buff, 0, size);

    (*buff)->size = SDDS_NET_MAX_BUF_SIZE;
    return SDDS_RT_OK;
}

rc_t
Network_getPayloadBegin(size_t* startByte) {
    // payload starts at the beginning of the buffer, address is provided
    // Separately
    // buffer starts after the struct
    *startByte = sizeof(struct NetFrameBuff_t);

    return SDDS_RT_OK;
}
size_t
Network_locSize(void) {
    return sizeof(RiotLocator_t);
}

rc_t
Network_setAddressToLocator(Locator_t* loc, char* addr) {
    if (loc == NULL || addr == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }
    RiotLocator_t* rloc = (RiotLocator_t*) loc;

    int ret = inet_pton(PTON_AF_INET6, addr, &(rloc->addr_storage.sin6_addr));
    if(ret != 1) {
        return SDDS_RT_FAIL;
    }
#ifdef MODULE_NATIVENET
    sixlowapp_ndp_workaround(&(rloc->addr_storage.sin6_addr));
#endif
    rloc->addr_storage.sin6_port = HTONS(PLATFORM_RIOT_SDDS_PORT);
    return SDDS_RT_OK;
}

rc_t
Network_setMulticastAddressToLocator(Locator_t* loc, char* addr) {
    if (loc == NULL || addr == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }
    RiotLocator_t* rloc = (RiotLocator_t*) loc;

    int ret = inet_pton(PTON_AF_INET6, addr, &(rloc->addr_storage.sin6_addr));
    if(ret != 1) {
        return SDDS_RT_FAIL;
    }
#ifdef MODULE_NATIVENET
    sixlowapp_ndp_workaround(&(rloc->addr_storage.sin6_addr));
#endif
    rloc->addr_storage.sin6_port = HTONS(PLATFORM_RIOT_SDDS_PORT + PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_PORT_OFF);
    return SDDS_RT_OK;
}

rc_t
Network_createLocator(Locator_t** loc) {
    *loc = Memory_alloc(sizeof(RiotLocator_t));

    if (*loc == NULL) {
        return SDDS_RT_NOMEM;
    }

    // set type for recvLoop
    (*loc)->type = SDDS_LOCATOR_TYPE_UNI;

    return Network_setAddressToLocator(*loc, PLATFORM_RIOT_SDDS_ADDRESS);
}

rc_t
Network_createMulticastLocator(Locator_t** loc) {
    *loc = Memory_alloc(sizeof(RiotLocator_t));
    if(*loc == NULL) {
        return SDDS_RT_NOMEM;
    }
    // set type for recvLoop
    (*loc)->type = SDDS_LOCATOR_TYPE_MULTI;

    return Network_setMulticastAddressToLocator(*loc, PLATFORM_RIOT_SDDS_BUILTIN_MULTICAST_ADDRESS);
}

bool_t
Locator_isEqual(Locator_t* l1, Locator_t* l2) {
    RiotLocator_t* a = (RiotLocator_t*)l1;
    RiotLocator_t* b = (RiotLocator_t*)l2;

    sockaddr6_t* addr[2];

    addr[0] = (sockaddr6_t*)&a->addr_storage;
    addr[1] = (sockaddr6_t*)&b->addr_storage;

    if (memcmp(&addr[0]->sin6_addr, &addr[1]->sin6_addr, 16) == 0) {
        return true;
    }
    else {
        return false;
    }
}


rc_t
Locator_getAddress(Locator_t* l, char* srcAddr) {
    RiotLocator_t* rloc = (RiotLocator_t*) l;
    char* ret = inet_ntop(PLATFORM_RIOT_SDDS_PROTOCOL, &(rloc->addr_storage.sin6_addr), srcAddr, IPV6_MAX_ADDR_STR_LEN);
    if(ret == NULL) {
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

#ifdef MODULE_NATIVENET
static void
sixlowapp_ndp_workaround(ipv6_addr_t* dest) {
    /* add the destination to the neighbor cache if is not already in it */
    if (!ndp_neighbor_cache_search(dest)) {
        DEBUGF("XXX: Adding %s to neighbor cache.\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest));
        ndp_neighbor_cache_add(SDDS_IF, dest, &(dest->uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE, 0xffff);
    }
}
#endif