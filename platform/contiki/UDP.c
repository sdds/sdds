#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/Log.h>
#include <sdds/LocatorDB.h>
#include <sdds/NetBuffRef.h>
#include <sdds/sdds_types.h>

#include <contiki.h>
#include <contiki-net.h>
#include <rpl.h>

#include <assert.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#ifndef PLATFORM_CONTIKI_SDDS_PORT
#define PLATFORM_CONTIKI_SDDS_PORT 23234
#endif

#define PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_ADDRESS 				SDDS_BUILTIN_MULTICAST_ADDRESS
#define PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS 	SDDS_BUILTIN_PARTICIPANT_ADDRESS
#define PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS 			SDDS_BUILTIN_TOPIC_ADDRESS
#define PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS 		SDDS_BUILTIN_SUB_PUB_ADDRESS

#define PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PORT_OFF 				20

#ifndef PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS
#define PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS "::"
#endif

#ifndef PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS
#define PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS "::1"
#endif

// IF BUILTIN
Locator g_builtin_topic_net_address;
// ENDIF

struct Contiki_Locator {
	struct Locator_t sdds_locator;
	uip_ipaddr_t address;
	uint16_t port;
};

typedef struct Contiki_Locator Contiki_Locator_t;
typedef struct Contiki_Locator* Contiki_Locator;

// global variables
struct simple_udp_connection g_connection;
struct simple_udp_connection g_multi_connection;
static struct NetBuffRef_t g_incoming_buffer;

void receive(struct simple_udp_connection *connection,
		uip_ipaddr_t const *src_addr, uint16_t src_port,
		uip_ipaddr_t const *dest_addr, uint16_t dest_port, uint8_t const *data,
		uint16_t data_len);

rc_t Network_Multicast_init();

rc_t Network_init(void) {
//	uip_ipaddr_t address;

//	uiplib_ipaddrconv(PLATFORM_CONTIKI_SDDS_LISTEN_ADDRESS, &address);

	NetBuffRef_init(&g_incoming_buffer);

//	mcast_conn = udp_new(NULL, UIP_HTONS(PLATFORM_CONTIKI_SDDS_PORT), NULL);

	// Funktioniert nicht mit multicast zusammen
	if (simple_udp_register(&g_connection, PLATFORM_CONTIKI_SDDS_PORT, NULL, 0,
			receive) != 1)
		return 1;

	if (simple_udp_register(&g_multi_connection, (PLATFORM_CONTIKI_SDDS_PORT+PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PORT_OFF), NULL, 0,
			receive) != 1)
		return 1;

#ifdef _MULTICAST
	Network_Multicast_init();
#endif

	return SDDS_RT_OK;
}

void receive(struct simple_udp_connection *connection,
		uip_ipaddr_t const *src_addr, uint16_t src_port,
		uip_ipaddr_t const *dest_addr, uint16_t dest_port, uint8_t const *data,
		uint16_t data_len) {
	Locator locator;
	Contiki_Locator_t this_locator;
	uint16_t i;

	/* make gcc happy */
	(void) connection;
	(void) dest_port;

	// ff02:0000:0000:0000:0000:0000:0000:0010
#ifdef UTILS_DEBUG
	char srcAddr[40];
	memset(srcAddr, 0 ,40);
	sprintf(srcAddr, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					((uint8_t *)dest_addr)[0], ((uint8_t *)dest_addr)[1], ((uint8_t *)dest_addr)[2], ((uint8_t *)dest_addr)[3],
					((uint8_t *)dest_addr)[4], ((uint8_t *)dest_addr)[5], ((uint8_t *)dest_addr)[6], ((uint8_t *)dest_addr)[7],
					((uint8_t *)dest_addr)[8], ((uint8_t *)dest_addr)[9], ((uint8_t *)dest_addr)[10], ((uint8_t *)dest_addr)[11],
					((uint8_t *)dest_addr)[12], ((uint8_t *)dest_addr)[13], ((uint8_t *)dest_addr)[14], ((uint8_t *)dest_addr)[15]);

	printf("---------> connection from: %s\n", srcAddr);
#endif

	NetBuffRef_renew(&g_incoming_buffer);

	if (data_len > 0 && data_len > g_incoming_buffer.frame_start->size) {
		printf("ist: %d, soll: %d\n", data_len, g_incoming_buffer.frame_start->size);
		Log_error("contiki: udp packet too big, discarded\n");
		return;
	}

	memcpy(g_incoming_buffer.buff_start, data, data_len);
	//uip_ipaddr_copy(&this_locator.address, &UIP_IP_BUF->srcipaddr);
	uip_ipaddr_copy(&this_locator.address, src_addr);
	//this_locator.port = UIP_UDP_BUF->srcport;
	this_locator.port = src_port;

	if (LocatorDB_findLocator((Locator) &this_locator, &locator) != SDDS_RT_OK) {
		// the locator is not yet present, create a new one
		Contiki_Locator contiki_locator;

		if (LocatorDB_newLocator(&locator) != SDDS_RT_OK) {
			Log_error("contiki: no free locator objects\n");
			return;
		}

		contiki_locator = (Contiki_Locator) locator;
		uip_ipaddr_copy(&contiki_locator->address, src_addr);
		contiki_locator->port = src_port;
	}

	// increase the reference count for the newly created locator
	Locator_upRef(locator);

	// make sure the locator is known as non-empty and sender
	locator->isEmpty = false;
	locator->isSender = true;

	g_incoming_buffer.addr = locator;

#ifdef UTILS_DEBUG
	int cmp = strcmp(srcAddr, "ff02:0000:0000:0000:0000:0000:0000:0010");
	if (cmp == 0) {
		Log_debug("contiki: recv_loop() data_len = %d\n", data_len);

		for (i = 0; i < data_len; i++) {
			if ((i%10) == 0) {
				printf("\n");
			}
			printf("0x%02x ", (uint8_t )g_incoming_buffer.buff_start[i]);
		}

		printf("\n");
	}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
	// process the frame
	DataSink_processFrame(&g_incoming_buffer);
#endif
	// and finally decrement the locator again
	Locator_downRef(locator);
}

rc_t Network_send(NetBuffRef buffer) {
	Locator loc = buffer->addr;
	struct uip_udp_conn *con;
	int port;

	while (loc != NULL) {
		uip_ipaddr_t address;
		Contiki_Locator locator;

		locator = (Contiki_Locator) loc;

		if (loc->type == SDDS_LOCATOR_TYPE_MULTI) {
			con = g_multi_connection.udp_conn;
			port = PLATFORM_CONTIKI_SDDS_PORT + PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PORT_OFF;
		}
		else if (loc->type == SDDS_LOCATOR_TYPE_UNI) {
			con = g_connection.udp_conn;
			port = PLATFORM_CONTIKI_SDDS_PORT;
		}

		address = locator->address;

		Log_debug("sending %d bytes to 0x%x buffer addr %p\n", buffer->curPos,
				locator->address.u8[15], buffer);

		uip_udp_packet_sendto(con, buffer->buff_start, buffer->curPos, &address, UIP_HTONS(port));

		loc = loc->next;
	}

	return SDDS_RT_OK;
}

bool_t Locator_isEqual(Locator locator1, Locator locator2) {
	Contiki_Locator contiki_locator_1;
	Contiki_Locator contiki_locator_2;

	contiki_locator_1 = (Contiki_Locator) locator1;
	contiki_locator_2 = (Contiki_Locator) locator2;

	// return true if address and port is equal
	return uip_ipaddr_cmp(&contiki_locator_1->address,
			&contiki_locator_2->address)
			&& contiki_locator_1->port == contiki_locator_2->port;
}

rc_t Network_getFrameBuff(NetFrameBuff* buffer) {
	size_t size;

	size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
	size += sizeof(struct NetFrameBuff_t);

	*buffer = Memory_alloc(size);

	if (*buffer == NULL) {
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}

	memset(*buffer, 0, size);
	(*buffer)->size = SDDS_NET_MAX_BUF_SIZE;

	return SDDS_RT_OK;
}

rc_t Network_getPayloadBegin(size_t *startByte) {
	*startByte = sizeof(struct NetFrameBuff_t);

	return SDDS_RT_OK;
}

rc_t Network_setAddressToLocator(Locator loc, char* addr) {

	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}
	Contiki_Locator l = (Contiki_Locator) loc;

	uiplib_ipaddrconv(addr, &l->address);
	l->port = UIP_HTONS(PLATFORM_CONTIKI_SDDS_PORT);

	return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator* loc) {
	Contiki_Locator contiki_locator;

	contiki_locator = Memory_alloc(sizeof(Contiki_Locator_t));

	if (contiki_locator == NULL) {
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}
	contiki_locator->sdds_locator.next = NULL;

	*loc = (Locator) contiki_locator;

	// set type for recvLoop
	(*loc)->type = SDDS_LOCATOR_TYPE_UNI;

	return Network_setAddressToLocator(*loc,
			PLATFORM_CONTIKI_SDDS_SEND_ADDRESSS);

}

rc_t Network_setMulticastAddressToLocator(Locator loc, char* addr) {
	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}
	Contiki_Locator l = (Contiki_Locator) loc;

	uiplib_ipaddrconv(addr, &l->address);
	l->port = UIP_HTONS(PLATFORM_CONTIKI_SDDS_PORT + PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PORT_OFF);

	return SDDS_RT_OK;
}

rc_t Network_createMulticastLocator(Locator* loc) {
	Contiki_Locator contiki_locator;

	contiki_locator = Memory_alloc(sizeof(Contiki_Locator_t));

	if (contiki_locator == NULL) {
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}
	contiki_locator->sdds_locator.next = NULL;

	*loc = (Locator) contiki_locator;

	// set type for recvLoop
	(*loc)->type = SDDS_LOCATOR_TYPE_MULTI;

	return Network_setMulticastAddressToLocator(*loc,
	PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_ADDRESS);
}

rc_t Network_Multicast_joinMulticastGroup(char *group) {
	uip_ipaddr_t addr;
	uip_ds6_maddr_t *rv;

	/*
	 * IPHC will use stateless multicast compression for this destination
	 * (M=1, DAC=0), with 32 inline bits (1E 89 AB CD)
	 */
	uiplib_ipaddrconv(group, &addr);
	rv = uip_ds6_maddr_add(&addr);

	if (rv) {
		PRINTF("Joined multicast group ");
		PRINT6ADDR(&uip_ds6_maddr_lookup(&addr)->ipaddr);
		PRINTF("\n");

		return SDDS_RT_OK;
	}

	return SDDS_RT_FAIL;
}

rc_t Network_Multicast_init() {
	uip_ipaddr_t addr;

	/* First, set our v6 global */
	uiplib_ipaddrconv("aaaa::0", &addr);
	uip_ds6_set_addr_iid(&addr, &uip_lladdr);
	uip_ds6_addr_add(&addr, 0, ADDR_AUTOCONF);

	rc_t ret;

	ret = Network_Multicast_joinMulticastGroup(PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_ADDRESS);
	if (ret != SDDS_RT_OK) {
		printf("ERROR\n");
	}

	ret = Network_Multicast_joinMulticastGroup(PLATFORM_CONTIKI_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS);
	if (ret != SDDS_RT_OK) {
		printf("ERROR\n");
	}

	return SDDS_RT_OK;
}

rc_t Locator_getAddress(Locator l, char *srcAddr) {
	uip_ipaddr_t *addr = &((Contiki_Locator) l)->address;
	sprintf(srcAddr, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
				((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3],
				((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7],
				((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11],
				((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15]);
	return SDDS_RT_OK;
}
