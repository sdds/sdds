#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include "SNPS.h"
#include "sdds_types.h"

typedef struct Discovery_Address {
	char addr[SDDS_SNPS_ADDR_SIZE+1];
	castType_t addrCast;
	addrType_t addrType;
} Discovery_Address_t;

rc_t Discovery_init();
rc_t Discovery_addParticipant(int participantID);
rc_t Discovery_handleParticipant(int participantID);
rc_t Discovery_addRemoteDataSink(char *addr, DDS_DataReader dataReader);

#endif	/* __DISCOVERY_H__ */
