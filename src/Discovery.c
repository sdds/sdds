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

#define SDDS_DISCOVERY_MAX_PARTICIPANTS 50

static SDDS_DCPSParticipant participants[SDDS_DISCOVERY_MAX_PARTICIPANTS];

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

rc_t Discovery_init() {
	for (int i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		participants[i].data.key=0;
	}
}

rc_t Discovery_addParticipant(SDDS_DCPSParticipant *p) {
	int i;
	int freePos = -1;

	if (BuiltinTopic_participantID == p->data.key) {
		return SDDS_RT_KNOWN;
	}

	for (i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
		if (participants[i].data.key == p->data.key) {
			return SDDS_RT_KNOWN;
		}
		else if ( (participants[i].data.key == 0) && (freePos == -1) ) {
			freePos = i;
		}
	}

	if (freePos != -1) {
		memcpy(&participants[freePos], p, sizeof(SDDS_DCPSParticipant));
		return SDDS_RT_OK;
	}

	return SDDS_RT_FAIL;
}

rc_t Discovery_handleParticipant(SDDS_DCPSParticipant p) {
	rc_t ret = 0;

	char srcAddr[1024];
	Locator_getAddress(p.addr, srcAddr);

	ret = Discovery_addParticipant(&p);

	if (ret == SDDS_RT_OK) printf( "add: %s\n", srcAddr);
	printRC(ret);
	if (ret == SDDS_RT_OK) {
#ifdef SDDS_TOPIC_HAS_PUB
		ret = Discovery_addRemoteDataSinkLoc(p.addr, g_DCPSPublication_topic);
#endif

#ifdef SDDS_TOPIC_HAS_SUB
		ret = Discovery_addRemoteDataSinkLoc(p.addr, g_DCPSSubscription_topic);
#endif
		if (ret != SDDS_RT_OK) {
			return ret;
		}

		Locator_downRef(p.addr);
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
