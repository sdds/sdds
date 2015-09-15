#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include "SNPS.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"
#include "DataSink.h"

struct DataReader;
typedef struct DataReader* DataReader;

rc_t Discovery_init();

#endif	/* __DISCOVERY_H__ */
