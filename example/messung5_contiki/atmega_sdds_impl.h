#ifndef ATMEGA_SDDS_IMPL_H_INC
#define ATMEGA_SDDS_IMPL_H_INC

#include "alpha-ds.h"
#include "beta-ds.h"

#include <dds/DDS_DCPS.h>

extern DDS_DataReader g_Alpha_reader;
extern DDS_DataWriter g_Alpha_writer;
extern DDS_Topic g_Alpha_topic;
//extern Alpha_data_t g_Alpha_pool[SDDS_TOPIC_APP_MSG_COUNT];

extern DDS_DataReader g_Beta_reader;
extern DDS_DataWriter g_Beta_writer;
extern DDS_Topic g_Beta_topic;
//extern Beta_data_t g_Beta_pool[SDDS_TOPIC_APP_MSG_COUNT];

//int init_sdds(int log_level);

#endif
