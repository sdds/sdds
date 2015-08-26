#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"
#include "DataSource.h"
#include "Discovery.h"
#include "Debug.h"
#include "DataSink.h"

#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>
#include <sdds/sDDS.h>
#include <sdds/Log.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#define SDDS_DISCOVERY_MAX_PARTICIPANTS 10

static int participants[SDDS_DISCOVERY_MAX_PARTICIPANTS];

// ******************* TEST ***********************
extern DDS_DataReader g_Beta_reader;
// ******************* TEST ***********************

rc_t Discovery_init() {
	memset(participants, 0, SDDS_DISCOVERY_MAX_PARTICIPANTS);
}

rc_t Discovery_addParticipant(int participantID) {
	int i;
	int freePos = -1;
	for (i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		if (participants[i] == participantID) {
			return SDDS_RT_KNOWN;
		}
		else if ( (participants[i] == 0) && (freePos == -1) ) {
			freePos = i;
		}
	}

	if (freePos != -1) {
		participants[freePos] = participantID;
		return SDDS_RT_OK;
	}

	return SDDS_RT_FAIL;
}

rc_t Discovery_handleParticipant(int participantID) {
	rc_t ret;
	Discovery_Address_t addr;

	ret = DataSink_getAddr(&addr);

	printf("======= receive Address =======\n");
	printf("castType: %u\n", addr.addrCast);
	printf("addrType: %u\n", addr.addrType);
	printf("addr: %s\n", addr.addr);

	ret = Discovery_addParticipant(participantID);
	printf("======= add participant =======\n");
	printRC(ret);
	if (ret == SDDS_RT_OK) {
		//ret = BuiltinTopic_addRemoteDataSinkToPubTopic(addr);
		ret = Discovery_addRemoteDataSink(addr.addr, g_Beta_reader);
	}

	return ret;
}

rc_t Discovery_addRemoteDataSink(char *addr, DDS_DataReader dataReader) {
	Locator l;
	rc_t ret;

	ret = LocatorDB_newLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

	ret = Network_setAddressToLocator(l, addr);
	if (ret != SDDS_RT_OK)
		return ret;

	ret = Topic_addRemoteDataSink(dataReader, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	return ret;
}
