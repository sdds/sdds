#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include "SNPS.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"
#include "DataSink.h"

struct DataReader;
typedef struct DataReader* DataReader;

rc_t Discovery_init();
rc_t Discovery_addParticipant(SDDS_DCPSParticipant *p);
rc_t Discovery_handleParticipant(SDDS_DCPSParticipant p);
rc_t Discovery_addRemoteDataSink(char *addr, Topic topic);
rc_t Discovery_addRemoteDataSinkLoc(Locator l, Topic topic);

#endif	/* __DISCOVERY_H__ */
