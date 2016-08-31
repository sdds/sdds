/*
 * ManagementTopic.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_MANAGEMENTTOPIC_H_
#define SDDS_INCLUDE_SDDS_MANAGEMENTTOPIC_H_

#include "sDDS.h"
#include "Locator.h"

#define SDDS_DCPS_MANAGEMENT_DOMAIN 0x1
#define SDDS_DCPS_MANAGEMENT_TOPIC 0x07

#ifndef SDDS_MANAGEMENT_TOPIC_KEY_SIZE
#define SDDS_MANAGEMENT_TOPIC_KEY_SIZE 255
#endif

#ifndef SDDS_MANAGEMENT_TOPIC_VALUE_SIZE
#define SDDS_MANAGEMENT_TOPIC_VALUE_SIZE 255
#endif

#define SDDS_MANAGEMENT_TOPIC_KEY_SET_SUBSCRIPTION_STATE    "set_subscription_state"
#define SDDS_MANAGEMENT_TOPIC_KEY_REQUEST_FILTER_EXPRESSION "request_filter_expression"
#define SDDS_MANAGEMENT_TOPIC_KEY_SEND_FILTER_EXPRESSION    "send_filter_expression"
#define SDDS_MANAGEMENT_TOPIC_KEY_DELETE_FILTER_EXPRESSION  "delete_filter_expression"

#define SDDS_MANAGEMENT_TOPIC_ALL_PARTICIPANTS              0

struct SDDS_DCPSManagement_t {
    union {
        BuiltinTopicKey_t key;
        BuiltinTopicKey_t pkey;
    };
    BuiltinTopicKey_t participant;
    DDS_char mKey[SDDS_MANAGEMENT_TOPIC_KEY_SIZE];
    DDS_char mValue[SDDS_MANAGEMENT_TOPIC_VALUE_SIZE];
    Locator_t* addr;
};
typedef struct SDDS_DCPSManagement_t SDDS_DCPSManagement;

extern DDS_DataReader g_DCPSManagement_reader;
extern DDS_DataWriter g_DCPSManagement_writer;
extern DDS_Topic g_DCPSManagement_topic;

DDS_ReturnCode_t
DDS_DCPSManagementDataReader_take_next_sample(
                                               DDS_DataReader slef,
                                               DDS_DCPSParticipant** data_values,
                                               DDS_SampleInfo* sample_info);

DDS_ReturnCode_t
DDS_DCPSManagementDataWriter_write(
                                    DDS_DataWriter self,
                                    const DDS_DCPSParticipant* instance_data,
                                    const DDS_InstanceHandle_t handle
                                    );

DDS_ReturnCode_t
SDDS_DCPSManagementDataWriter_writeToParticipant(
                                    DDS_DataWriter* self,
                                    const DDS_DCPSParticipant* instance_data,
                                    const DDS_InstanceHandle_t handle,
                                    Locator_t* addr
                                    );

rc_t
ManagementTopic_init(void);

#endif /* SDDS_INCLUDE_SDDS_MANAGEMENTTOPIC_H_ */
