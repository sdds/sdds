/*
 * =====================================================================================
 *
 *       Filename:  UDP.c
 *
 *    Description:  impl for network layer as a UDP service
 *
 *        Version:  1.0
 *        Created:  22.02.2010 16:10:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "Locator.h"
#include "LocatorDB.h"
#include "Network.h"
#include "Log.h"
#include "os-ssal/Memory.h"
#include "NetBuffRef.h"
#include "DataSink.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"
#include "gen_constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <net/if.h>

// taking into account ipv4 tunneling features
#define PLATFORM_LINUX_IPV6_MAX_CHAR_LEN 45

#ifndef PLATFORM_LINUX_SDDS_PORT
#define PLATFORM_LINUX_SDDS_PORT 23234
#endif

#ifndef PLATFORM_LINUX_SDDS_PROTOCOL
#define PLATFORM_LINUX_SDDS_PROTOCOL AF_INET6
#endif

#define PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_ADDRESS 				SDDS_BUILTIN_MULTICAST_ADDRESS
#define PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS 	SDDS_BUILTIN_PARTICIPANT_ADDRESS
#define PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS 		SDDS_BUILTIN_TOPIC_ADDRESS
#define PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS 		SDDS_BUILTIN_SUB_PUB_ADDRESS

#define PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PORT_OFF 20
#define PLATFORM_LINUX_MULTICAST_SO_RCVBUF 1200000

#ifndef PLATFORM_LINUX_SDDS_LISTEN_ADDRESS
#define PLATFORM_LINUX_SDDS_LISTEN_ADDRESS "::"
#endif

#ifndef PLATFORM_LINUX_SDDS_SEND_ADDRESS
#define PLATFORM_LINUX_SDDS_SEND_ADDRESS "::1"
#endif

//#define PRINT_RECVBUF 1

struct Network_t {
	int fd_uni_socket;
	int fd_multi_socket;
	int port;
	pthread_t recvThread;
	pthread_t multiRecvThread;
};

struct UDPLocator_t {
	struct Locator_t loc;
	struct sockaddr_storage addr_storage;
};

static struct Network_t net;
static struct NetBuffRef_t inBuff;
static struct NetBuffRef_t multiInBuff;

pthread_mutex_t recv_mutex = PTHREAD_MUTEX_INITIALIZER;

void* recvLoop(void*);
static int create_socket(struct addrinfo *address);

// for the builtintopic
// IF BUILTIN

Locator builtinTopicNetAddress;
// ENDIF

size_t Network_size(void) {
	return sizeof(struct Network_t);
}

rc_t Network_Multicast_joinMulticastGroup(char *group) {
	struct addrinfo *mReq; /* Multicast Address */
	char multicastPort[PLATFORM_LINUX_IPV6_MAX_CHAR_LEN];
	struct addrinfo hints = { 0 }; /* Hints for name lookup */
	int status;

	sprintf(multicastPort, "%d", (net.port + PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PORT_OFF));

	/* Join the multicast group. We do this seperately depending on whether we
	 * are using IPv4 or IPv6.
	 */
	struct ipv6_mreq multicastRequest; /* Multicast address join structure */

	if ((status = getaddrinfo(group, multicastPort, &hints,
				&mReq)) != 0) {
		Log_error("ERROR: setsockopt() failed\n");
		return SDDS_RT_FAIL;
	}

	/* Specify the multicast group */
	memcpy(&multicastRequest.ipv6mr_multiaddr,
			&((struct sockaddr_in6*) (mReq->ai_addr))->sin6_addr,
			sizeof(multicastRequest.ipv6mr_multiaddr));

	/* Accept multicast from any interface */
	if ((multicastRequest.ipv6mr_interface = if_nametoindex("usb0")) < 0)
	{
		printf("Ignoring unknown interface: %s: %s\n", "usb0", strerror(errno));
		multicastRequest.ipv6mr_interface = 0;
	}

	/* Join the multicast address */
	if (setsockopt(net.fd_multi_socket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
			(char*) &multicastRequest, sizeof(multicastRequest)) != 0) {
		Log_error("ERROR: setsockopt() failed\n");
		return SDDS_RT_FAIL;
	}

	return SDDS_RT_OK;
}

rc_t Network_Multicast_init() {
	struct addrinfo* multicastAddr; /* Multicast address */
	struct addrinfo* localAddr; /* Local address to bind to */
	char* multicastIP = PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_ADDRESS; /* Arg: IP Multicast address */
	char multicastPort[PLATFORM_LINUX_IPV6_MAX_CHAR_LEN];
	int multicastTTL; /* Arg: TTL of multicast packets */
	struct addrinfo hints = { 0 }; /* Hints for name lookup */

	sprintf(multicastPort, "%d", (net.port + PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PORT_OFF));

	multicastTTL = 1;

	/* Resolve destination address for multicast datagrams */
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_NUMERICHOST;
	int status;
	if ((status = getaddrinfo(PLATFORM_LINUX_SDDS_SEND_ADDRESS, multicastPort, &hints,
			&multicastAddr)) != 0) {
		Log_error("%d ERROR: getaddrinfo() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}

	printf("Using %s\n",
			multicastAddr->ai_family == PF_INET6 ? "IPv6" : "IPv4");

	/* Create socket for sending multicast datagrams */
	if ((net.fd_multi_socket = socket(multicastAddr->ai_family, multicastAddr->ai_socktype, 0))
			< 0) {
		Log_error("%d ERROR: socket() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}

	/* select sender interface */
	unsigned int outif;
	if ((outif = if_nametoindex(PLATFORM_LINUX_SDDS_IFACE)) < 0)
	{
		printf("Ignoring unknown interface: %s: %s\n", PLATFORM_LINUX_SDDS_IFACE, strerror(errno));
		outif = 0;
	}
	if (setsockopt(net.fd_multi_socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, &outif, sizeof(outif)) != 0) {
		printf("Could not join the multicast group: %s\n", strerror(errno));
		return SDDS_RT_FAIL;;
	}

	/* Set TTL of multicast packet */
	if (setsockopt(net.fd_multi_socket,
			multicastAddr->ai_family == PF_INET6 ? IPPROTO_IPV6 : IPPROTO_IP,
			multicastAddr->ai_family == PF_INET6 ?
			IPV6_MULTICAST_HOPS :
													IP_MULTICAST_TTL,
			(char*) &multicastTTL, sizeof(multicastTTL)) != 0) {
		Log_error("%d ERROR: setsockopt() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}


	/* Get a local address with the same family (IPv4 or IPv6) as our multicast group */
	hints.ai_family = multicastAddr->ai_family;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; /* Return an address we can bind to */

	if (getaddrinfo(NULL, multicastPort, &hints, &localAddr) != 0) {
		Log_error("%d ERROR: getaddrinfo() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}

	int yes = 1;
	/* lose the pesky "Address already in use" error message */
	if (setsockopt(net.fd_multi_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(int))
			== -1) {
		Log_error("%d ERROR: setsockopt() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}

	/* Bind to the multicast port */
	if (bind(net.fd_multi_socket, localAddr->ai_addr, localAddr->ai_addrlen) != 0) {
		Log_error("%d ERROR: bind() failed: %s\n", __LINE__, strerror(errno));
		return SDDS_RT_FAIL;
	}

	/* get/set socket receive buffer */
	int optval = 0;
	socklen_t optval_len = sizeof(optval);
	int dfltrcvbuf;
	if (getsockopt(net.fd_multi_socket, SOL_SOCKET, SO_RCVBUF, (char*) &optval, &optval_len)
			!= 0) {
		Log_error("%d ERROR: getsockopt() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}
	dfltrcvbuf = optval;
	optval = PLATFORM_LINUX_MULTICAST_SO_RCVBUF;
	if (setsockopt(net.fd_multi_socket, SOL_SOCKET, SO_RCVBUF, (char*) &optval, sizeof(optval))
			!= 0) {
		Log_error("%d ERROR: setsockopt() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}
	if (getsockopt(net.fd_multi_socket, SOL_SOCKET, SO_RCVBUF, (char*) &optval, &optval_len)
			!= 0) {
		Log_error("%d ERROR: getsockopt() failed\n", __LINE__);
		return SDDS_RT_FAIL;
	}
	printf("tried to set socket receive buffer from %d to %d, got %d\n",
			dfltrcvbuf, PLATFORM_LINUX_MULTICAST_SO_RCVBUF, optval);

	Network_Multicast_joinMulticastGroup(PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_ADDRESS);
	Network_Multicast_joinMulticastGroup(PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS);
	Network_Multicast_joinMulticastGroup(PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS);
	Network_Multicast_joinMulticastGroup(PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS);

	NetBuffRef_init(&multiInBuff);
	Network_createMulticastLocator(&multiInBuff.addr);
	// set up a thread to read from the udp multicast socket
	if (pthread_create(&net.multiRecvThread, NULL, recvLoop,
			(void *) &multiInBuff) != 0) {
		exit(-1);
	}

	freeaddrinfo(multicastAddr);
	freeaddrinfo(localAddr);

	return SDDS_RT_OK;
}


rc_t Network_init(void) {

	struct addrinfo* address;
	struct addrinfo hints;
	char port_buffer[6];

	net.port = PLATFORM_LINUX_SDDS_PORT;

	// clear hints, no dangling fields
	memset(&hints, 0, sizeof hints);

	// getaddrinfo wants its port parameter in string form
	sprintf(port_buffer, "%u", net.port);

	// returned addresses will be used to create datagram sockets
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = PLATFORM_LINUX_SDDS_PROTOCOL;
	hints.ai_flags = AI_PASSIVE;

	int gai_ret = getaddrinfo(PLATFORM_LINUX_SDDS_SEND_ADDRESS, port_buffer,
			&hints, &address);
	if (gai_ret != 0) {
		Log_error("can't obtain suitable addresses for listening\n");
		return -1;
	}
	//printf("%i bytes empfangen\n", (int)recv_size);
	// implicit call of the network receive handler
	// should start from now ;)

	// search in the db for the locator
	// TODO do something better than this hack here....
	net.fd_uni_socket = create_socket(address);

	if (net.fd_uni_socket < 0) {
		freeaddrinfo(address);
		return -1;
	}

	// free up address
	freeaddrinfo(address);

	// init the incoming frame buffer and add dummy unicast locator
	NetBuffRef_init(&inBuff);
	Network_createLocator(&inBuff.addr);

	// set up a thread to read from the udp socket
	if (pthread_create(&net.recvThread, NULL, recvLoop, (void *) &inBuff)
			!= 0) {
		exit(-1);
	}

	// IF BUILTINTOPIC
	builtinTopicNetAddress = Memory_alloc(sizeof(struct UDPLocator_t));

	if (builtinTopicNetAddress == NULL) {
		return SDDS_RT_NOMEM;
	}

	struct UDPLocator_t *l = (struct UDPLocator_t *) builtinTopicNetAddress;
	l->loc.next = NULL;

	// save broadcast address for ipv4 or multicast for ipv6
#if PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET
	struct sockaddr_in *addr = (struct sockaddr_in *)&l->addr_storage;

	inet_aton("255.255.255.255", &addr->sin_addr);
	addr->sin_port = htons(net.port);
#elif PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET6
	struct sockaddr_in6 *addr = (struct sockaddr_in6 *) &l->addr_storage;

	addr->sin6_family = AF_INET6;
	addr->sin6_addr.s6_addr[0] = 0xff;
	addr->sin6_addr.s6_addr[1] = 0x02;
	addr->sin6_addr.s6_addr[15] = 1;
	addr->sin6_port = htons(net.port);
#else
#error "Only AF_INET and AF_INET6 are understood linux protocols."
#endif
	// ENDIF

#ifdef _MULTICAST
	Network_Multicast_init();
#endif

	return SDDS_RT_OK;
}

static int create_socket(struct addrinfo *address) {
	int fd = socket(address->ai_family, address->ai_socktype, 0);

	if (fd < 0) {
		Log_error("can't create socket\n");
		return -1;
	}

	assert(address->ai_family == PLATFORM_LINUX_SDDS_PROTOCOL);

#if PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET
	// IF NET AND BROADCAST
	int broadcast_permission = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof broadcast_permission) < 0)
	{
		Log_error("can't prepare IPv4 socket for broadcasting\n");
		close(fd);
		return -1;
	}
	// ENDIF
#elif PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET6
	// IPv6 doesn't support broadcasting, use multicast when sending packets
#else
#error "Only AF_INET and AF_INET6 are understood linux protocols."
#endif

	// bind the socket
	if (bind(fd, address->ai_addr, address->ai_addrlen) != 0) {
		Log_error("Unable to bind socket\n");
		return -1;
	}

#ifdef UTILS_DEBUG
	// show which address was assigned
	{
		char address_buffer[NI_MAXHOST];

		if (
				getnameinfo(
						address->ai_addr,
						address->ai_addrlen,
						address_buffer,
						NI_MAXHOST,
						NULL,
						0,
						NI_NUMERICHOST
				) != 0)
		{
			// ignore getnameinfo errors, just for debugging anyway
		}
		else
		{
			Log_debug("listening on [%s]:%u for udp packets\n", address_buffer, net.port);
		}
	}
#endif

	return fd;
}

void *recvLoop(void *netBuff) {
	NetBuffRef buff = (NetBuffRef) netBuff;
	int sock; // receive socket
	unsigned int sock_type; // broad or multicast

	int count = 0;

	// Check the dummy locator for uni or multicast socket
	struct Locator_t *l = (struct Locator_t *) buff->addr;
	sock_type = l->type;

	if (sock_type == SDDS_LOCATOR_TYPE_MULTI) {
		sock = net.fd_multi_socket;
		Log_info("Receive on multicast socket\n");
	} else if (sock_type == SDDS_LOCATOR_TYPE_UNI) {
		sock = net.fd_uni_socket;
		Log_info("Receive on unicast socket\n");
	}

	while (true) {
		//reset the buffer
		NetBuffRef_renew(buff);

		// spare address field?
		struct sockaddr_storage addr;
		socklen_t addr_len = sizeof(addr);

		ssize_t recv_size = recvfrom(sock, buff->buff_start,
				buff->frame_start->size, 0,
				(struct sockaddr *)&addr, &addr_len);

		if (recv_size == -1) {
			Log_error("Error while receiving a udp frame \n");
			continue;
		}

		printf("[%u]%i bytes empfangen\n", sock_type, (int) recv_size);

		// implicit call of the network receive handler
		// should start from now ;)

		// search in the db for the locator
		// TODO do something better than this hack here....

		struct UDPLocator_t sloc;

		memcpy(&(sloc.addr_storage), &addr, addr_len);

		Locator loc;

		//pthread_mutex_lock(&recv_mutex);
		if (LocatorDB_findLocator((Locator) &sloc, &loc) != SDDS_RT_OK) {
			// not found we need a new one
			if (LocatorDB_newLocator(&loc) != SDDS_RT_OK) {
				Log_error("No free Locator objects\n");
				continue;
			}

			memcpy(&((struct UDPLocator_t *) loc)->addr_storage, &addr, addr_len);
		}

		// up ref counter
		Locator_upRef(loc);

		//pthread_mutex_unlock(&recv_mutex);

		loc->isEmpty = false;
		loc->isSender = true;
		loc->type = sock_type;

		// TODO use function buffer
		// add locator to the netbuffref
		buff->addr = loc;
//		if (sock_type == SDDS_LOCATOR_TYPE_MULTI)
//			multiInBuff.addr = loc;
//		else if (sock_type == SDDS_LOCATOR_TYPE_UNI)
//			inBuff.addr = loc;

#ifdef PRINT_RECVBUF
		printf("recvBuffer: \n");
		for (int i =0; i< recv_size; i++) {

			for (ssize_t i = 0; i < recv_size; i++)
			{
				printf("0x%02x ", (uint8_t)buff->buff_start[i]);
			}

			printf("\n");
		}
#endif

		//pthread_mutex_lock(&recv_mutex);

		// invoke the datasink handler

		DataSink_processFrame(buff);

		LocatorDB_freeLocator(loc);

	}

	//pthread_mutex_unlock(&recv_mutex);

	return SDDS_RT_OK;
}

rc_t Network_send(NetBuffRef buff) {
	int sock;
	unsigned int sock_type;

	// Check the locator for uni or multicast socket
	struct Locator_t *l = (struct Locator_t *) buff->addr;
	sock_type = l->type;
	// add locator to the netbuffref
	if (sock_type == SDDS_LOCATOR_TYPE_MULTI)
		sock = net.fd_multi_socket;
	else if (sock_type == SDDS_LOCATOR_TYPE_UNI)
		sock = net.fd_uni_socket;

	Locator loc = buff->addr;

//	printf("========== send NetBuff ==========\n");
//	NetBuffRef_print(buff);
//	printf("========== END NetBuff =======\n");

	while (loc != NULL) {

		ssize_t transmitted;
		struct sockaddr *addr =
				(struct sockaddr *) &((struct UDPLocator_t *) loc)->addr_storage;

		transmitted = sendto(sock, buff->buff_start, buff->curPos, 0, addr,
				sizeof(struct sockaddr_storage));

		if (transmitted == -1) {
			perror("ERROR:");
			Log_error("can't send udp packet\n");
		} else {
			Log_debug("Transmitted %d bytes over %s\n", transmitted,
					sock_type == SDDS_LOCATOR_TYPE_UNI ? "unicast" : "multicast");
		}

		loc = loc->next;
	}

	return SDDS_RT_OK;
}

void Network_recvFrameHandler(Network _this) {
}

rc_t Network_getFrameBuff(NetFrameBuff* buff) {
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

rc_t Network_getPayloadBegin(size_t* startByte) {
	// payload starts at the beginning of the buffer, address is provided
	// Separately
	// buffer starts after the struct
	*startByte = sizeof(struct NetFrameBuff_t);

	return SDDS_RT_OK;
}
size_t Network_locSize(void) {
	return sizeof(struct UDPLocator_t);
}

rc_t Network_setAddressToLocator(Locator loc, char* addr) {

	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}

	struct UDPLocator_t* l = (struct UDPLocator_t*) loc;

	struct addrinfo *address;
	struct addrinfo hints;
	char port_buffer[6];

	// clear hints, no dangling fields
	memset(&hints, 0, sizeof hints);

	// getaddrinfo wants its port parameter in string form
	sprintf(port_buffer, "%u", net.port);

	// returned addresses will be used to create datagram sockets
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = PLATFORM_LINUX_SDDS_PROTOCOL;

	int gai_ret = getaddrinfo(addr, port_buffer, &hints, &address);

	if (gai_ret != 0) {
		Log_error(
				"can't obtain suitable addresses %s for setting UDP locator\n",
				addr);

		return SDDS_RT_FAIL;
	}

#ifdef UTILS_DEBUG
	// show which address was assigned
	{
		char address_buffer[NI_MAXHOST];

		if (
				getnameinfo(
						address->ai_addr,
						address->ai_addrlen,
						address_buffer,
						NI_MAXHOST,
						NULL,
						0,
						NI_NUMERICHOST
				) != 0)
		{
			// ignore getnameinfo errors, just for debugging anyway
		}
		else
		{
			Log_debug("created a locator for [%s]:%u\n", address_buffer, net.port);
		}
	}
#endif

	memcpy(&l->addr_storage, address->ai_addr, address->ai_addrlen);

	// free up address
	freeaddrinfo(address);

	return SDDS_RT_OK;
}

rc_t Network_setMulticastAddressToLocator(Locator loc, char* addr) {

	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}

	struct UDPLocator_t* l = (struct UDPLocator_t*) loc;

	struct addrinfo *address;
	struct addrinfo hints;
	char port_buffer[6];

	// clear hints, no dangling fields
	memset(&hints, 0, sizeof hints);

	// getaddrinfo wants its port parameter in string form
	sprintf(port_buffer, "%u",
			net.port + PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_PORT_OFF);

	// returned addresses will be used to create datagram sockets
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = PLATFORM_LINUX_SDDS_PROTOCOL;

	int gai_ret = getaddrinfo(addr, port_buffer, &hints, &address);

	if (gai_ret != 0) {
		Log_error(
				"can't obtain suitable addresses %s for setting UDP locator\n",
				addr);

		return SDDS_RT_FAIL;
	}

#ifdef UTILS_DEBUG
	// show which address was assigned
	{
		char address_buffer[NI_MAXHOST];

		if (
				getnameinfo(
						address->ai_addr,
						address->ai_addrlen,
						address_buffer,
						NI_MAXHOST,
						NULL,
						0,
						NI_NUMERICHOST
				) != 0)
		{
			// ignore getnameinfo errors, just for debugging anyway
		}
		else
		{
			Log_debug("created a locator for [%s]:%s\n", address_buffer, port_buffer);
		}
	}
#endif

	memcpy(&l->addr_storage, address->ai_addr, address->ai_addrlen);

	// free up address
	freeaddrinfo(address);

	return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator* loc) {

	*loc = Memory_alloc(sizeof(struct UDPLocator_t));

	if (*loc == NULL) {
		return SDDS_RT_NOMEM;
	}

	// set type for recvLoop
	(*loc)->type = SDDS_LOCATOR_TYPE_UNI;

	return Network_setAddressToLocator(*loc,
	PLATFORM_LINUX_SDDS_SEND_ADDRESS);
}

rc_t Network_createMulticastLocator(Locator* loc) {

	*loc = Memory_alloc(sizeof(struct UDPLocator_t));

	if (*loc == NULL) {
		return SDDS_RT_NOMEM;
	}

	// set type for recvLoop
	(*loc)->type = SDDS_LOCATOR_TYPE_MULTI;

	return Network_setMulticastAddressToLocator(*loc,
	PLATFORM_LINUX_SDDS_BUILTIN_MULTICAST_ADDRESS);
}

bool_t Locator_isEqual(Locator l1, Locator l2) {
	struct UDPLocator_t *a = (struct UDPLocator_t *) l1;
	struct UDPLocator_t *b = (struct UDPLocator_t *) l2;
#if PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET
	struct sockaddr_in *addr[2];

	addr[0] = (struct sockaddr_in *)&a->addr_storage;
	addr[1] = (struct sockaddr_in *)&b->addr_storage;

	if (memcmp(&addr[0]->sin_addr.s_addr, &addr[1]->sin_addr.s_addr, 4) == 0)
	{
		return true;
	} else {
		return false;
	}
#elif PLATFORM_LINUX_SDDS_PROTOCOL == AF_INET6

	struct sockaddr_in6 *addr[2];

	addr[0] = (struct sockaddr_in6 *) &a->addr_storage;
	addr[1] = (struct sockaddr_in6 *) &b->addr_storage;

	if (memcmp(&addr[0]->sin6_addr.s6_addr, &addr[1]->sin6_addr.s6_addr, 16)
			== 0) {
		return true;
	} else {
		return false;
	}
#endif
}

rc_t Locator_getAddress(Locator l, char *srcAddr) {
	static char srcPort[NI_MAXSERV];
	struct sockaddr_storage *a = &((struct UDPLocator_t *) l)->addr_storage;

	int rc = getnameinfo((struct sockaddr *) a, sizeof(struct sockaddr_storage), srcAddr,
			NI_MAXHOST, srcPort, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

	return SDDS_RT_OK;
}

