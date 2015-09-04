#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include "SNPS.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"
#include "DataSink.h"

#include <netdb.h>

#define SDDS_DISCOVERY_ADDR_STR_LENGTH	NI_MAXHOST	//SDDS_SNPS_ADDR_SIZE

struct Discovery_Address {
	char addr[SDDS_DISCOVERY_ADDR_STR_LENGTH+1];
	castType_t addrCast;
	addrType_t addrType;
};

typedef struct Discovery_Address Discovery_Address_t;

struct DataReader;
typedef struct DataReader* DataReader;

rc_t Discovery_init();
rc_t Discovery_addParticipant(int participantID);
rc_t Discovery_handleParticipant(int participantID);
rc_t Discovery_addRemoteDataSink(char *addr, Topic topic);
rc_t Discovery_addRemoteDataSinkLoc(Locator l, Topic topic);

#endif	/* __DISCOVERY_H__ */
