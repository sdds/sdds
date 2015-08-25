#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include "SNPS.h"
#include "sdds_types.h"

#define SDDS_DISCOVERY_ADDR_SIZE	16

typedef struct Discovery_address {
	char addr[SDDS_DISCOVERY_ADDR_SIZE];
	castType_t addrCast;
	addrType_t addrType;
} Discovery_address_t;

rc_t Discovery_init();
rc_t Discovery_addParticipant(int participantID);

#endif	/* __DISCOVERY_H__ */
