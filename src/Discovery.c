#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"
#include "DataSource.h"
#include "Discovery.h"

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
