#ifndef DELTA_H_INC
#define DELTA_H_INC

#include <dds/DDS_DCPS.h>

#include <sdds/Topic.h>

struct Delta
{
	DDS_char value;
};

typedef struct Delta Delta;

#ifdef SDDS_TOPIC_HAS_PUB
DDS_ReturnCode_t DDS_DeltaDataReader_take_next_sample(
	DDS_DataReader _this,
	Delta** values,
	DDS_SampleInfo* sample_info
);

#endif

#ifdef SDDS_TOPIC_HAS_SUB
DDS_ReturnCode_t DDS_DeltaDataWriter_write(
	DDS_DataWriter _this,
	const Delta* instance_data,
	const DDS_InstanceHandle_t handle
);
#endif

Topic sDDS_DeltaTopic_create(Delta* pool, int count);

#endif
