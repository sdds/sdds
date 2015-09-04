#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"
#include "DataSource.h"
#include "Discovery.h"
#include "Debug.h"
#include "DataSink.h"

#include "os-ssal/NodeConfig.h"

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

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

rc_t Discovery_init() {
	memset(participants, 0, SDDS_DISCOVERY_MAX_PARTICIPANTS);
}

rc_t Discovery_addParticipant(int participantID) {
	int i;
	int freePos = -1;

//	printf("me: %d\n", NodeConfig_getNodeID());

	if (BuiltinTopic_participantID == participantID) {
		return SDDS_RT_KNOWN;
	}

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
	ret = Discovery_addParticipant(participantID);

	if (ret == SDDS_RT_OK) printf( "add: %s\n", addr.addr);
	printRC(ret);
	if (ret == SDDS_RT_OK) {
#ifdef SDDS_TOPIC_HAS_PUB
		ret = BuiltinTopic_addRemoteDataSinkToPubTopic(addr);
#endif

#ifdef SDDS_TOPIC_HAS_SUB
		ret = BuiltinTopic_addRemoteDataSinkToSubTopic(addr);
#endif
	}

	return ret;
}

rc_t Discovery_addRemoteDataSink(char *addr, Topic topic) {
	Locator l;
	rc_t ret;

	ret = LocatorDB_newLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

	ret = Network_setAddressToLocator(l, addr);
	if (ret != SDDS_RT_OK)
		return ret;

	ret = Topic_addRemoteDataSink(topic, l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_downRef(l);

	return ret;
}

rc_t Discovery_addRemoteDataSinkLoc(Locator l, Topic topic) {
	rc_t ret;

	ret = Topic_addRemoteDataSink(topic, l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_downRef(l);

	return ret;
}

#endif
