#ifndef GAMMA_H_INC
#define GAMMA_H_INC

#include <dds/DDS_DCPS.h>

#include <sdds/Topic.h>

struct Gamma
{
	DDS_char value;
};

typedef struct Gamma Gamma;

#ifdef SDDS_TOPIC_HAS_PUB
DDS_ReturnCode_t DDS_GammaDataReader_take_next_sample(
	DDS_DataReader _this,
	Gamma** values,
	DDS_SampleInfo* sample_info
);

#endif

#ifdef SDDS_TOPIC_HAS_SUB
DDS_ReturnCode_t DDS_GammaDataWriter_write(
	DDS_DataWriter _this,
	const Gamma* instance_data,
	const DDS_InstanceHandle_t handle
);
#endif

Topic sDDS_GammaTopic_create(Gamma* pool, int count);

#endif
