#ifndef BETA_H_INC
#define BETA_H_INC

#include <dds/DDS_DCPS.h>

#include <sdds/Topic.h>

struct Beta
{
	DDS_char value;
};

typedef struct Beta Beta;

#ifdef SDDS_TOPIC_HAS_PUB
DDS_ReturnCode_t DDS_BetaDataReader_take_next_sample(
	DDS_DataReader _this,
	Beta** values,
	DDS_SampleInfo* sample_info
);

#endif

#ifdef SDDS_TOPIC_HAS_SUB
DDS_ReturnCode_t DDS_BetaDataWriter_write(
	DDS_DataWriter _this,
	const Beta* instance_data,
	const DDS_InstanceHandle_t handle
);
#endif

Topic sDDS_BetaTopic_create(Beta* pool, int count);

#endif
