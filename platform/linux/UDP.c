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
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>

#ifndef SDDS_LINUX_PORT
#define SDDS_LINUX_PORT 23234
#endif

#ifndef SDDS_LINUX_PROTOCOL
// only use AF_INET or AF_INET6
#define SDDS_LINUX_PROTOCOL AF_INET6
#endif

#ifndef SDDS_LINUX_LISTEN_ADDRESS
#define SDDS_LINUX_LISTEN_ADDRESS "::"
#endif

#ifndef SDDS_LINUX_SEND_ADDRESS
#define SDDS_LINUX_SEND_ADDRESS "::1"
#endif

struct Network_t {
    int fd_uni_socket;
    int port;
    pthread_t recvThread;
};

struct UDPLocator_t
{
	struct Locator_t loc;
	struct sockaddr_storage addr_storage;
};

static struct Network_t net;
static struct NetBuffRef_t inBuff;

void* recvLoop(void*);
static int create_socket(struct addrinfo *address);

// for the builtintopic
// IF BUILTIN

Locator builtinTopicNetAddress;
// ENDIF



size_t Network_size(void)
{
    return sizeof(struct Network_t);
}
rc_t Network_init(void)
{
	struct addrinfo* address;
	struct addrinfo hints;
	char port_buffer[6];

	net.port = SDDS_LINUX_PORT;

	// clear hints, no dangling fields
	memset(&hints, 0, sizeof hints);

	// getaddrinfo wants its port parameter in string form
	sprintf(port_buffer, "%u", net.port);

	// returned addresses will be used to create datagram sockets
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = SDDS_LINUX_PROTOCOL;
	hints.ai_flags = AI_PASSIVE;

	int gai_ret = getaddrinfo(NULL, port_buffer, &hints, &address);
	if (gai_ret != 0)
	{
		Log_error("can't obtain suitable addresses for listening\n");
		return -1;
	}
	//printf("%i bytes empfangen\n", (int)recv_size);
	// implicit call of the network receive handler
	// should start from now ;)

	// search in the db for the locator
	// TODO do something better than this hack here....
	net.fd_uni_socket = create_socket(address);

	if (net.fd_uni_socket < 0)
	{
		freeaddrinfo(address);
		return -1;
	}

	// free up address
	freeaddrinfo(address);

	// init the incoming frame buffer
	NetBuffRef_init(&inBuff);

	// set up a thread to read from the udp socket
	if (pthread_create(&net.recvThread, NULL, recvLoop, (void *)&inBuff) != 0)
	{
		exit(-1);
	}

	// IF BUILTINTOPIC
	builtinTopicNetAddress = Memory_alloc(sizeof(struct UDPLocator_t));

	if (builtinTopicNetAddress == NULL)
	{
		return SDDS_RT_NOMEM;
	}

	struct UDPLocator_t *l = (struct UDPLocator_t *)builtinTopicNetAddress;
	l->loc.next = NULL;

	// save broadcast address for ipv4 or multicast for ipv6
#if SDDS_LINUX_PROTOCOL == AF_INET
	struct sockaddr_in *addr = (struct sockaddr_in *)&l->addr_storage;

	inet_aton("255.255.255.255", &addr->sin_addr);
	addr->sin_port = htons(net.port);
#elif SDDS_LINUX_PROTOCOL == AF_INET6
	struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&l->addr_storage;

	addr->sin6_family = AF_INET6;
	addr->sin6_addr.s6_addr[0] = 0xff;
	addr->sin6_addr.s6_addr[1] = 0x02;
	addr->sin6_addr.s6_addr[15] = 1;
	addr->sin6_port = htons(net.port);
#else
#error "Only AF_INET and AF_INET6 are understood linux protocols."
#endif
	// ENDIF

	return SDDS_RT_OK;
}


static int create_socket(struct addrinfo *address)
{
	int fd = socket(address->ai_family, address->ai_socktype, 0);

	if (fd < 0)
	{
		Log_error("can't create socket\n");
		return -1;
	}

	assert(address->ai_family == SDDS_LINUX_PROTOCOL);

#if SDDS_LINUX_PROTOCOL == AF_INET
	// IF NET AND BROADCAST
	int broadcast_permission = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof broadcast_permission) < 0)
	{
		Log_error("can't prepare IPv4 socket for broadcasting\n");
		close(fd);
		return -1;
	}
	// ENDIF
#elif SDDS_LINUX_PROTOCOL == AF_INET6
	// IPv6 doesn't support broadcasting, use multicast when sending packets
#else
#error "Only AF_INET and AF_INET6 are understood linux protocols."
#endif

	// bind the socket
	if (bind(fd, address->ai_addr, address->ai_addrlen) != 0)
	{
		Log_error("Unable to bind socket\n");
		return -1;
	}

#ifdef _DEBUG
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



void *recvLoop(void *netBuff)
{
	NetBuffRef buff = (NetBuffRef)netBuff;

    while (true)
    {
		//reset the buffer
		NetBuffRef_renew(buff);

		// spare address field?
		struct sockaddr_storage addr;
		socklen_t addr_len = sizeof(addr);
		ssize_t recv_size = recvfrom(net.fd_uni_socket, buff->buff_start,
		                             buff->frame_start->size, 0,
		                             (struct sockaddr *)&addr, &addr_len);

		if (recv_size == -1)
		{
			Log_error("Error while receiving a udp frame \n");
			continue;
		}

		//printf("%i bytes empfangen\n", (int)recv_size);

		// implicit call of the network receive handler
		// should start from now ;)

		// search in the db for the locator
		// TODO do something better than this hack here....



		struct UDPLocator_t sloc;

		memcpy(&(sloc.addr_storage), &addr, addr_len);

		Locator loc;

		if (LocatorDB_findLocator((Locator)&sloc, &loc) != SDDS_RT_OK)
		{
			// not found we need a new one
			if (LocatorDB_newLocator(&loc) != SDDS_RT_OK)
			{
				Log_error("No free Locator objects\n");
				continue;
			}

			memcpy(&((struct UDPLocator_t *)loc)->addr_storage, &addr, addr_len);
		}

		// up ref counter
		Locator_upRef(loc);

		loc->isEmpty = false;
		loc->isSender = true;

		// add locator to the netbuffref
		inBuff.addr = loc;


#if PRINT_RECVBUF
	printf("recvBuffer: \n");
	for (int i =0; i< recv_size; i++){

		for (ssize_t i = 0; i < recv_size; i++)
		{
			printf("0x%02x ", (uint8_t)buff->buff_start[i]);
		}

		printf("\n");
#endif

	// invoke the datasink handler
	DataSink_processFrame(buff);

	LocatorDB_freeLocator(loc);

    }

    return SDDS_RT_OK;
}

rc_t Network_send(NetBuffRef buff) {

	Locator loc = buff->addr;

	while (loc != NULL ) {

		ssize_t transmitted;
		struct sockaddr *addr =
				(struct sockaddr *) &((struct UDPLocator_t *) loc)->addr_storage;

		transmitted = sendto(net.fd_uni_socket, buff->buff_start, buff->curPos,
				0, addr, sizeof(struct sockaddr_storage));

		if (transmitted == -1) {
			Log_error("can't send udp packet\n");
		} else {
			Log_debug("Transmitted %d bytes \n", transmitted);
		}

		loc = loc->next;
	}

	return SDDS_RT_OK;
}

void Network_recvFrameHandler(Network _this)
{
}


rc_t Network_getFrameBuff(NetFrameBuff* buff)
{
	size_t size = sDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
	size += sizeof(struct NetFrameBuff_t);

	*buff = Memory_alloc(size);

    if (*buff == NULL){
    	return SDDS_RT_NOMEM;
    }
    memset(*buff, 0, size);

    (*buff)->size = sDDS_NET_MAX_BUF_SIZE;
    return SDDS_RT_OK;
}

rc_t Network_getPayloadBegin(size_t* startByte)
{
    // payload starts at the beginning of the buffer, address is provided
    // Separately
    // buffer starts after the struct
    *startByte = sizeof(struct NetFrameBuff_t);

    return SDDS_RT_OK;
}
size_t Network_locSize(void)
{
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
	hints.ai_family = SDDS_LINUX_PROTOCOL;

	int gai_ret = getaddrinfo(addr, port_buffer, &hints, &address);

	if (gai_ret != 0)
	{
		Log_error("can't obtain suitable addresses for setting UDP locator\n");

		return SDDS_RT_FAIL;
	}


#ifdef _DEBUG
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

rc_t Network_createLocator(Locator* loc)
{

    *loc = Memory_alloc(sizeof(struct UDPLocator_t));

	if (*loc == NULL)
	{
    	return SDDS_RT_NOMEM;
    }

	return Network_setAddressToLocator(*loc, SDDS_LINUX_SEND_ADDRESS);

}

bool_t Locator_isEqual(Locator l1, Locator l2)
{
	struct UDPLocator_t *a = (struct UDPLocator_t *)l1;
	struct UDPLocator_t *b = (struct UDPLocator_t *)l2;
#if SDDS_LINUX_PROTOCOL == AF_INET
	struct sockaddr_in *addr[2];

	addr[0] = (struct sockaddr_in *)&a->addr_storage;
	addr[1] = (struct sockaddr_in *)&b->addr_storage;

	if (memcmp(&addr[0]->sin_addr.s_addr, &addr[1]->sin_addr.s_addr, 4) == 0)
	{
		return true;
	} else {
		return false;
	}
#elif SDDS_LINUX_PROTOCOL == AF_INET6

	struct sockaddr_in6 *addr[2];

	addr[0] = (struct sockaddr_in6 *)&a->addr_storage;
	addr[1] = (struct sockaddr_in6 *)&b->addr_storage;

	if (memcmp(&addr[0]->sin6_addr.s6_addr, &addr[1]->sin6_addr.s6_addr, 16) == 0)
	{
		return true;
	} else {
	    return false;
    }
#endif
}

