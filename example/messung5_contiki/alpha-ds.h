#ifndef ALPHA_H_INC
#define ALPHA_H_INC

#include <dds/DDS_DCPS.h>

#include <sdds/Topic.h>

struct Alpha
{
	DDS_char value;
};

typedef struct Alpha Alpha;

#ifdef SDDS_TOPIC_HAS_PUB
DDS_ReturnCode_t DDS_AlphaDataReader_take_next_sample(
	DDS_DataReader _this,
	Alpha** values,
	DDS_SampleInfo* sample_info
);

#endif

#ifdef SDDS_TOPIC_HAS_SUB
DDS_ReturnCode_t DDS_AlphaDataWriter_write(
	DDS_DataWriter _this,
	const Alpha* instance_data,
	const DDS_InstanceHandle_t handle
);
#endif

Topic sDDS_AlphaTopic_create(Alpha* pool, int count);

#endif
