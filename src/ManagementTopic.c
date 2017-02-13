/*
 * ManagementTopic.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "ManagementTopic.h"
#include "ManagementTopicDataWriter.h"

#ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED

DDS_Topic g_DCPSManagement_topic;
Sample_t dcps_management_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSManagement dcps_management_sample_data[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSManagement dcps_management_incomingSample_data;
DDS_DataReader g_DCPSManagement_reader;
DDS_DataWriter g_DCPSManagement_writer;

void*
TopicMarshalling_DCPSManagement_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSManagement_getSecondaryKey(Data data);

rc_t
TopicMarshalling_DCPSManagement_cmpPrimaryKeys(Data data1, Data data2);

rc_t
TopicMarshalling_DCPSManagement_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSManagement_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSManagementDataReader_take_next_sample(
                                               DDS_DataReader self,
                                               SDDS_DCPSManagement** data_values,
                                               DDS_SampleInfo* sample_info
                                               ) {
    rc_t ret = DataReader_take_next_sample((DataReader_t*) self, (Data*) data_values, (DataInfo) sample_info);

    if (ret == SDDS_RT_NODATA) {
        return DDS_RETCODE_NO_DATA;
    }

    if (ret == SDDS_RT_OK) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

DDS_ReturnCode_t
DDS_DCPSManagementDataWriter_write(
                                    DDS_DataWriter self,
                                    const SDDS_DCPSManagement* instance_data,
                                    const DDS_InstanceHandle_t handle
                                    ) {
    rc_t ret = DataWriter_write((DataWriter_t*) self, (Data)instance_data, (void*) handle);
    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr,"{SCL:M}\n");
#endif
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

DDS_ReturnCode_t
SDDS_DCPSManagementDataWriter_writeToParticipant(
                                    DDS_DataWriter* self,
                                    const SDDS_DCPSManagement* instance_data,
                                    const DDS_InstanceHandle_t handle,
                                    Locator_t* addr
                                    ) {
    rc_t ret = ManagementTopicDataWriter_writeToParticipant((DataWriter_t*) self, (Data)instance_data, (void*) handle, addr);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr,"{SCL:M}\n");
#endif
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

void*
TopicMarshalling_DCPSManagement_getPrimaryKey(Data data) {
    SDDS_DCPSManagement* real_data = (SDDS_DCPSManagement*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSManagement_getSecondaryKey(Data data) {
    return NULL;
}


rc_t
TopicMarshalling_DCPSManagement_cmpPrimaryKeys(Data data1, Data data2) {
    SDDS_DCPSManagement* real_data1 = (SDDS_DCPSManagement*) data1;
    SDDS_DCPSManagement* real_data2 = (SDDS_DCPSManagement*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

rc_t
TopicMarshalling_DCPSManagement_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(SDDS_DCPSManagement));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSManagement_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;

    SDDS_DCPSManagement* real_data = (SDDS_DCPSManagement*) data;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);
    maxSize += sizeof(real_data->participant);
    maxSize += SDDS_MANAGEMENT_TOPIC_KEY_SIZE;
    maxSize += SDDS_MANAGEMENT_TOPIC_VALUE_SIZE;

    byte_t* start = buffer->buff_start + buffer->curPos + 1;

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_enc_uint16(start + *size, &real_data->participant);
    *size += sizeof(real_data->participant);

    Marshalling_enc_string(start + *size, real_data->mKey, SDDS_MANAGEMENT_TOPIC_KEY_SIZE);
    *size += SDDS_MANAGEMENT_TOPIC_KEY_SIZE;

    Marshalling_enc_string(start + *size, real_data->mValue, SDDS_MANAGEMENT_TOPIC_VALUE_SIZE);
    *size += SDDS_MANAGEMENT_TOPIC_VALUE_SIZE;

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSManagement_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    SDDS_DCPSManagement* real_data = (SDDS_DCPSManagement*) data;

    size_t expectedSize = 0;
    expectedSize += sizeof(real_data->key);
    expectedSize += sizeof(real_data->participant);
    expectedSize += SDDS_MANAGEMENT_TOPIC_KEY_SIZE;
    expectedSize += SDDS_MANAGEMENT_TOPIC_VALUE_SIZE;

    if (*size != expectedSize) {
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_uint16(start + *size, &real_data->participant);
    *size += sizeof(real_data->participant);

    Marshalling_dec_string(start + *size, real_data->mKey, SDDS_MANAGEMENT_TOPIC_KEY_SIZE);
    *size += SDDS_MANAGEMENT_TOPIC_KEY_SIZE;

    Marshalling_dec_string(start + *size, real_data->mValue, SDDS_MANAGEMENT_TOPIC_VALUE_SIZE);
    *size += SDDS_MANAGEMENT_TOPIC_VALUE_SIZE;

    real_data->addr = buffer->locators->first_fn(buffer->locators);
    assert(real_data->addr);
    Locator_upRef(real_data->addr);

    return SDDS_RT_OK;
}

Topic_t*
sDDS_DCPSManagementTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_management_incomingSample_data;
    topic->Data_encode = TopicMarshalling_DCPSManagement_encode;
    topic->Data_decode = TopicMarshalling_DCPSManagement_decode;

    topic->domain = SDDS_DCPS_MANAGEMENT_DOMAIN;
    topic->id = SDDS_DCPS_MANAGEMENT_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSManagement_cpy;
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSManagement_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSManagement_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSManagement_getSecondaryKey;

    topic->dsinks.list = List_initConcurrentLinkedList();
    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

rc_t
ManagementTopic_init(void) {
    int index;
    for (index = 0; index < SDDS_QOS_HISTORY_DEPTH; index++) {
        dcps_management_samples_pool[index].data = &dcps_management_sample_data[index];
    }

    g_DCPSManagement_topic = sDDS_DCPSManagementTopic_create();
    if (g_DCPSManagement_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSManagement_reader = DataSink_create_datareader(g_DCPSManagement_topic, NULL, NULL, NULL);
    if (g_DCPSManagement_reader == NULL){
        return SDDS_RT_FAIL;
    }
    sdds_History_setup (DataReader_history (g_DCPSManagement_reader), dcps_management_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSManagement_writer = DataSource_create_datawriter(g_DCPSManagement_topic, NULL, NULL, NULL);
    if (g_DCPSManagement_writer == NULL){
        return SDDS_RT_FAIL;
    }

    return SDDS_RT_OK;
}

#endif
