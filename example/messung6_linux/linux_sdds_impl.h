#ifndef LINUX_SDDS_IMPL_H_INC
#define LINUX_SDDS_IMPL_H_INC

#include "alpha-ds.h"
#include "beta-ds.h"
#include "gamma-ds.h"
#include "delta-ds.h"

#include <dds/DDS_DCPS.h>

extern DDS_DataReader g_Alpha_reader;
extern DDS_DataWriter g_Alpha_writer;
extern DDS_Topic g_Alpha_topic;
//extern Alpha_data_t g_Alpha_pool[SDDS_TOPIC_APP_MSG_COUNT];

extern DDS_DataReader g_Beta_reader;
extern DDS_DataWriter g_Beta_writer;
extern DDS_Topic g_Beta_topic;
//extern Beta_data_t g_Beta_pool[SDDS_TOPIC_APP_MSG_COUNT];

extern DDS_DataReader g_Gamma_reader;
extern DDS_DataWriter g_Gamma_writer;
extern DDS_Topic g_Gamma_topic;
//extern Gamma_data_t g_Gamma_pool[SDDS_TOPIC_APP_MSG_COUNT];

extern DDS_DataReader g_Delta_reader;
extern DDS_DataWriter g_Delta_writer;
extern DDS_Topic g_Delta_topic;
//extern Delta_data_t g_Delta_pool[SDDS_TOPIC_APP_MSG_COUNT];

//int init_sdds(int log_level);

#endif
