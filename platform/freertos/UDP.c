#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/Log.h>
#include <sdds/LocatorDB.h>
#include <sdds/NetBuffRef.h>
#include <sdds/sdds_types.h>
#include <string.h>

#include "lwip/udp.h"

#define PLATFORM_RTOS_SDDS_BUILTIN_MULTICAST_ADDRESS 				SDDS_BUILTIN_MULTICAST_ADDRESS
#define PLATFORM_RTOS_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS 	SDDS_BUILTIN_PARTICIPANT_ADDRESS
#define PLATFORM_RTOS_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS 			SDDS_BUILTIN_TOPIC_ADDRESS
#define PLATFORM_RTOS_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS 		SDDS_BUILTIN_SUB_PUB_ADDRESS

struct FreeRTOS_Locator
{
	Locator_t sdds_locator;
	struct ip_addr address;
	uint16_t port;
};

typedef struct FreeRTOS_Locator FreeRTOS_Locator_t;
typedef struct FreeRTOS_Locator* FreeRTOS_Locator;

static NetBuffRef_t incoming_buffer;

struct udp_pcb *unicastConn_pcb;
struct pbuf *p;

void recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port){
	Locator_t* locator;
	FreeRTOS_Locator_t this_locator;
	uint16_t i;

	NetBuffRef_renew(&incoming_buffer);

	if (p->len > 0 && p->len > incoming_buffer.frame_start->size)
	{
		Log_error("contiki: udp packet too big, discarded\n");
		return;
	}

	memcpy(incoming_buffer.buff_start, p->payload, p->len);
	//uip_ipaddr_copy(&this_locator.address, &UIP_IP_BUF->srcipaddr);
	ip_addr_set(&this_locator.address, addr);
	//this_locator.port = UIP_UDP_BUF->srcport;
	this_locator.port = port;

	if (LocatorDB_findLocator((Locator_t*)&this_locator, &locator) != SDDS_RT_OK)
	{
		// the locator is not yet present, create a new one
		FreeRTOS_Locator freertos_locator;

		if (LocatorDB_newLocator(&locator) != SDDS_RT_OK)
		{
			Log_error("contiki: no free locator objects\n");
			return;
		}

		freertos_locator = (FreeRTOS_Locator)locator;
		ip_addr_set(&freertos_locator->address, addr);
		freertos_locator->port = port;
	}

	// increase the reference count for the newly created locator
	Locator_upRef(locator);

	// make sure the locator is known as non-empty and sender
	locator->isEmpty = false;
	locator->isSender = true;

	incoming_buffer.addr = locator;

	Log_debug("contiki: recv_loop() buffer: ");

	for (i = 0; i < p->len; i++)
	{
		Log_debug("0x%02x ", (uint8_t)incoming_buffer.buff_start[i]);
	}

	Log_debug("\n");

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
	// process the frame
	DataSink_processFrame(&incoming_buffer);
#endif
	// and finally decrement the locator again
	Locator_downRef(locator);

	 pbuf_free(p);
}

rc_t Network_init(void) {
	unicastConn_pcb = udp_new();

    udp_bind(unicastConn_pcb, IP_ADDR_ANY, PLATFORM_RTOS_SDDS_PORT);
    udp_recv(unicastConn_pcb, recv, NULL);

    NetBuffRef_init(&incoming_buffer);

	return SDDS_RT_OK;
}

rc_t Network_send(NetBuffRef_t *buffer) {
	Locator_t* loc = buffer->addr;

	while (loc != NULL ) {
		struct ip_addr address;
		FreeRTOS_Locator locator;

		locator = (FreeRTOS_Locator) loc;

		address = locator->address;

		Log_debug("sending %d bytes to 0x%x buffer addr %d\n",
				buffer->curPos, address.addr, buffer);

		p = pbuf_alloc(PBUF_TRANSPORT, buffer->curPos, PBUF_RAM);
		memcpy (p->payload, buffer->buff_start, buffer->curPos);
		udp_sendto(unicastConn_pcb, p, &address, PLATFORM_RTOS_SDDS_PORT);
		pbuf_free(p);

		loc = loc->next;
	}

	return SDDS_RT_OK;
}

bool_t Locator_isEqual(Locator_t* locator1, Locator_t* locator2) {
	FreeRTOS_Locator freertos_locator_1;
	FreeRTOS_Locator freertos_locator_2;

	freertos_locator_1 = (FreeRTOS_Locator) locator1;
	freertos_locator_2 = (FreeRTOS_Locator) locator2;

	// return true if address and port is equal
	return (ip_addr_cmp(&freertos_locator_1->address, &freertos_locator_2->address)
			&&
		   (freertos_locator_1->port == freertos_locator_2->port));

	return SDDS_RT_OK;
}

rc_t Network_getFrameBuff(NetFrameBuff* buffer) {

	size_t size;

	size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
	size += sizeof(struct NetFrameBuff_t);

	*buffer = Memory_alloc(size);

	if (*buffer == NULL)
	{
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

rc_t Network_setAddressToLocator(Locator_t* loc, char* addr) {
	if (loc == NULL || addr == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}
	FreeRTOS_Locator l = (FreeRTOS_Locator) loc;

	// IPV4 only!
	char buffer[16];
	uint8_t intAddr[4];
	char* token = NULL;

	strcpy(buffer, addr);

	token = strtok(buffer, ".");
	intAddr[0] = atoi(token);

	token = strtok(NULL, ".");
	intAddr[1] = atoi(token);

	token = strtok(NULL, ".");
	intAddr[2] = atoi(token);

	token = strtok(NULL, "\0");
	intAddr[3] = atoi(token);

	IP4_ADDR(&l->address, intAddr[0], intAddr[1], intAddr[2], intAddr[3]);
	l->port = htons(PLATFORM_RTOS_SDDS_PORT);

	return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator_t** locator) {
	FreeRTOS_Locator freertos_locator;

	freertos_locator = Memory_alloc(sizeof(FreeRTOS_Locator_t));

	if (freertos_locator == NULL)
	{
		Log_error("contiki: out of memory\n");
		return SDDS_RT_NOMEM;
	}
	freertos_locator->sdds_locator.next = NULL;

	*locator = (Locator_t*) freertos_locator;

	return Network_setAddressToLocator(*locator, PLATFORM_RTOS_SDDS_ADDRESS);
}

rc_t Network_createMulticastLocator(Locator_t** loc) {
	return Network_setMulticastAddressToLocator(*loc,
	PLATFORM_RTOS_SDDS_BUILTIN_MULTICAST_ADDRESS);
}

rc_t Network_setMulticastAddressToLocator(Locator_t* loc, char* addr) {
	return SDDS_RT_OK;
}

rc_t Locator_getAddress(Locator_t* l, char *srcAddr) {
	return SDDS_RT_OK;
}
