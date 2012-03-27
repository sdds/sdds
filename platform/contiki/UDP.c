#include <os-ssal/Memory.h>

#include <sDDS/DataSink.h>
#include <sDDS/Log.h>
#include <sDDS/LocatorDB.h>
#include <sDDS/NetBuffRef.h>
#include <sDDS/sdds_types.h>

#include <contiki.h>
#include <contiki-net.h>
#include <net/simple-udp.h>
#include <net/uip-ds6.h>

#include <assert.h>

#define UIP_IP_BUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#ifndef SDDS_CONTIKI_PORT
#define SDDS_CONTIKI_PORT 23234
#endif

#ifndef SDDS_CONTIKI_LISTEN_ADDRESS
#define SDDS_CONTIKI_LISTEN_ADDRESS "::"
#endif

#ifndef SDDS_CONTIKI_SEND_ADDRESS
#define SDDS_CONTIKI_SEND_ADDRESS "::1"
#endif

// IF BUILTIN
Locator g_builtin_topic_net_address;
// ENDIF

struct udp_locator
{
	struct Locator_t sdds_locator;
	uip_ipaddr_t address;
	u16_t port;
};

typedef struct udp_locator udp_locator_t;

// global variables
struct simple_udp_connection g_connection;
static struct NetBuffRef_t g_incoming_buffer;

void receive(struct simple_udp_connection *connection,
             uip_ipaddr_t const *src_addr, uint16_t src_port,
             uip_ipaddr_t const *dest_addr, uint16_t dest_port,
             uint8_t const *data, uint16_t data_len);

rc_t Network_init(void)
{
	uip_ipaddr_t address;

	uiplib_ipaddrconv(SDDS_CONTIKI_LISTEN_ADDRESS, &address);

	NetBuffRef_init(&g_incoming_buffer);

	if (simple_udp_register(&g_connection, SDDS_CONTIKI_PORT, &address, 0, receive) != 1)
		return 1;

	return SDDS_RT_OK;
}

void receive(struct simple_udp_connection *connection,
             uip_ipaddr_t const *src_addr, uint16_t src_port,
             uip_ipaddr_t const *dest_addr, uint16_t dest_port,
             uint8_t const *data, uint16_t data_len)
{
	Locator locator;
	udp_locator_t this_locator;
	uint16_t i;

	/* make gcc happy */
	(void)connection;
	(void)dest_addr;
	(void)dest_port;

	NetBuffRef_renew(&g_incoming_buffer);

	if (data_len > 0 && data_len > g_incoming_buffer.frame_start->size)
	{
		Log_error("contiki: udp packet too big, discarded\n");
		return;
	}

	memcpy(g_incoming_buffer.buff_start, uip_appdata, length);
	uip_ipaddr_copy(&this_locator.address, &UIP_IP_BUF->srcipaddr);
	this_locator.port = UIP_UDP_BUF->srcport;

	if (LocatorDB_findLocator((Locator)&this_locator, &locator) != SDDS_RT_OK)
	{
		// the locator is not yet present, create a new one
		udp_locator_t *udp_locator;

		if (LocatorDB_newLocator(&locator) != SDDS_RT_OK)
		{
			Log_error("contiki: no free locator objects\n");
			return;
		}

		udp_locator = (udp_locator_t *)locator;
		uip_ipaddr_copy(&udp_locator->address, &UIP_IP_BUF->srcipaddr);
		udp_locator->port = UIP_UDP_BUF->srcport;
	}

	// increase the reference count for the newly created locator
	Locator_upRef(locator);

	// make sure the locator is known as non-empty and sender
	locator->isEmpty = false;
	locator->isSender = true;

	g_incoming_buffer.addr = locator;

	Log_debug("contiki: recv_loop() buffer: ");

	for (i = 0; i < data_len; i++)
	{
		Log_debug("0x%02x ", (uint8_t)g_incoming_buffer.buff_start[i]);
	}

	Log_debug("\n");

#ifdef sDDS_TOPIC_HAS_PUB
	// process the frame
	DataSink_processFrame(&g_incoming_buffer);
#endif
	// and finally decrement the locator again
	Locator_downRef(locator);
}

rc_t Network_send(NetBuffRef buffer)
{
	int result;
	uip_ipaddr_t address;
	udp_locator_t *locator;

	locator = (udp_locator_t *)buffer->addr;

	// multicast on all nodes link-local
	//uip_ip6addr(&address, 0xff02, 0, 0, 0, 0, 0, 0, 1);
	address = locator->address;

	uip_udp_packet_sendto(g_connection.udp_conn, buffer->buff_start, buffer->curPos,
	                      &address, UIP_HTONS(SDDS_CONTIKI_PORT));

	return SDDS_RT_OK;
}

bool_t Locator_isEqual(Locator locator1, Locator locator2)
{
	udp_locator_t *udp_locator_1;
	udp_locator_t *udp_locator_2;

	udp_locator_1 = (udp_locator_t *)locator1;
	udp_locator_2 = (udp_locator_t *)locator2;

	// return true if address and port is equal
	return uip_ipaddr_cmp(&udp_locator_1->address, &udp_locator_2->address) && \
	       udp_locator_1->port == udp_locator_2->port;
}

rc_t Network_getFrameBuff(NetFrameBuff* buffer)
{
	size_t size;

	size = sDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
	size += sizeof(struct NetFrameBuff_t);

	*buffer = Memory_alloc(size);

	if (*buffer == NULL)
	{
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}

	memset(*buffer, 0, size);
	(*buffer)->size = sDDS_NET_MAX_BUF_SIZE;

	return SDDS_RT_OK;
}

rc_t Network_getPayloadBegin(size_t *startByte)
{
	*startByte = sizeof(struct NetFrameBuff_t);

	return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator* locator)
{
	udp_locator_t *udp_locator;

	*locator = Memory_alloc(sizeof(udp_locator_t));

	if (!*locator)
	{
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}

	udp_locator = (udp_locator_t *)*locator;
	udp_locator->sdds_locator.next = NULL;
	uiplib_ipaddrconv(SDDS_CONTIKI_SEND_ADDRESS, &udp_locator->address);
	udp_locator->port = UIP_HTONS(SDDS_CONTIKI_PORT);

	return SDDS_RT_OK;
}
