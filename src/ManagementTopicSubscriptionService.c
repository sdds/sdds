/*
 * ManagementTopicSubscriptionService.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "ManagementTopicSubscriptionService.h"
#include "ManagementTopicPublicationService.h"
#include "ManagementTopic.h"
#include "string.h"
#include "stdint.h"

#ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED

#define SDDS_MANAGEMENT_TOPIC_ALL_PARTICIPANTS           0

#define SDDS_SUBSCRIPTION_REACTIVATION_TIMER_SEC         1

#ifndef SDDS_SUBSCRIPTION_REACTIVATION_TIMER_MSEC
#define SDDS_SUBSCRIPTION_REACTIVATION_TIMER_MSEC        500
#endif


static void
s_executeManagementDirective(DDS_DataReader reader);

rc_t
s_key_setSubscriptionState(DDS_char* mValue);

rc_t
s_key_requestFilterExpression(DDS_char* mValue, Locator_t* addr);

rc_t
s_key_deleteFilterExpression(DDS_char* mValue);

static Task reactivateSubscriptionTask;

static void
s_reactivateSubscription();

rc_t
ManagementTopicSubscriptionService_init() {
#if defined(SDDS_TOPIC_HAS_SUB)
#   if (SDDS_SUBSCRIPTION_REACTIVATION_TIMER_MSEC != 0 || SDDS_SUBSCRIPTION_REACTIVATION_TIMER_SEC != 0)
    reactivateSubscriptionTask = Task_create();
    Task_init(reactivateSubscriptionTask, s_reactivateSubscription, NULL);
    if (Task_start(reactivateSubscriptionTask, SDDS_SUBSCRIPTION_REACTIVATION_TIMER_SEC, SDDS_SUBSCRIPTION_REACTIVATION_TIMER_MSEC, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif
#endif

    DDS_ReturnCode_t dds_ret;
    struct DDS_DataReaderListener managementListStruct = {
            .on_data_available = &s_executeManagementDirective
    };

    dds_ret = DDS_DataReader_set_listener(g_DCPSManagement_reader, &managementListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set executeManagementDirective.\n");
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

static void
s_executeManagementDirective(DDS_DataReader reader) {
    DDS_ReturnCode_t ret;
    SDDS_DCPSManagement m_data_used;
    SDDS_DCPSManagement* m_data_used_ptr = &m_data_used;

    do {
        ret = DDS_DCPSManagementDataReader_take_next_sample(reader, (SDDS_DCPSManagement**) &m_data_used_ptr, NULL);
        if (ret == DDS_RETCODE_OK) {
            if (m_data_used.participant == SDDS_MANAGEMENT_TOPIC_ALL_PARTICIPANTS || m_data_used.participant == BuiltinTopic_participantID) {
                if (strcmp(m_data_used.mKey, SDDS_MANAGEMENT_TOPIC_KEY_SET_SUBSCRIPTION_STATE) == 0) {
#ifdef SDDS_TOPIC_HAS_SUB
                    printf("Received ManagementDirective: %s\n", m_data_used.mKey);
                    if (s_key_setSubscriptionState(m_data_used.mValue) != SDDS_RT_OK) {
                        Log_error("Unable to set subscription state.\n");
                    }
#endif
                }
                else if (strcmp(m_data_used.mKey, SDDS_MANAGEMENT_TOPIC_KEY_REQUEST_FILTER_EXPRESSION) == 0) {
                    printf("Received ManagementDirective: %s to ", m_data_used.mKey);
                    Locator_print(m_data_used.addr);
                    if (s_key_requestFilterExpression(m_data_used.mValue, m_data_used.addr) != SDDS_RT_OK) {
                        Log_error("Unable to process FilterExpression request.\n");
                    }
                }
                else if (strcmp(m_data_used.mKey, SDDS_MANAGEMENT_TOPIC_KEY_SEND_FILTER_EXPRESSION) == 0) {
#ifdef SDDS_SUBSCRIPTION_MANAGER
                    printf("Received ManagementDirective: %s\n", m_data_used.mKey);
                    if (SubscriptionManagementService_registerFilter(&m_data_used) != SDDS_RT_OK) {
                        Log_error("Unable to process FilterExpression.\n");
                    }
#endif
                }
                else if (strcmp(m_data_used.mKey, SDDS_MANAGEMENT_TOPIC_KEY_DELETE_FILTER_EXPRESSION) == 0) {
#ifdef SDDS_SUBSCRIPTION_MANAGER
                    printf("Received ManagementDirective: %s\n", m_data_used.mKey);
                    if (s_key_deleteFilterExpression(m_data_used.mValue) != SDDS_RT_OK) {
                        Log_error("Unable to delete FilterExpression.\n");
                    }
#endif
                }
            }
        }
    } while ((ret != DDS_RETCODE_NO_DATA) && (ret != DDS_RETCODE_ERROR));
}

/*
 * mValue format: topicid_t;SSW_NodeID_t;SubscriptionState_t
 */

rc_t
s_key_setSubscriptionState(DDS_char* mValue) {
    int size = 0;

    topicid_t topicID;
    rc_t ret =  Marshalling_dec_uint8((byte_t*) mValue + size, &topicID);
    size += sizeof(topicid_t);

    BuiltinTopicKey_t participant;
    ret =  Marshalling_dec_uint16((byte_t*) mValue + size, &participant);
    size += sizeof(BuiltinTopicKey_t);


    uint8_t st;
    ret =  Marshalling_dec_uint8((byte_t*) mValue + size, &st);
    size += sizeof(st);
    SubscriptionState_t state = st;

    Topic_t* topic = TopicDB_getTopic(topicID);

    List_t* subscriptions = topic->dsinks.list;

    TopicSubscription_t* sub = subscriptions->first_fn(subscriptions);
    while (sub != NULL) {
        if (sub->participant == participant) {
            break;
        }
        sub = subscriptions->next_fn(subscriptions);
    }

    if (sub == NULL) {
        Log_error("Subscription not found.\n");
        return SDDS_RT_FAIL;
    }


    sub->state = state;

    if (Time_getTime16(&sub->updated) != SDDS_SSW_RT_OK) {
        Log_error("Unable to set time.\n");
        return SDDS_RT_FAIL;
    }

    printf("Subscription updated (%x): t:%d, %d, %u\n", sub->participant, topic->id, sub->state, sub->updated);

    return SDDS_RT_OK;
}

/*
 * mValue format: topicid_t
 */

rc_t
s_key_requestFilterExpression(DDS_char* mValue, Locator_t* addr) {
    topicid_t topicID;
    rc_t ret =  Marshalling_dec_uint8((byte_t*) mValue, &topicID);

    Topic_t* topic = TopicDB_getTopic(topicID);
    if (topic == NULL) {
        Log_error("Unknown topic: %s\n", mValue);
        return SDDS_RT_FAIL;
    }

    FilteredDataReader_t* filteredReader = DataSink_getFilteredDataReaders();
    for (int i = 0; i < SDDS_DATA_FILTER_READER_MAX_OBJS; i++) {
        if (filteredReader[i].locationFilteredTopic->contentFilteredTopic.associatedTopic->id == topicID) {

            SDDS_DCPSManagement data;
            data.participant = SDDS_MANAGEMENT_TOPIC_ALL_PARTICIPANTS;
            strcpy(data.mKey, SDDS_MANAGEMENT_TOPIC_KEY_SEND_FILTER_EXPRESSION);

            int size = 0;
            Marshalling_enc_uint8(data.mValue+size, &topicID);
            size += sizeof(topicID);

            Marshalling_enc_uint16(data.mValue+size, &BuiltinTopic_participantID);
            size += sizeof(BuiltinTopic_participantID);

            Marshalling_enc_uint8(data.mValue+size, &filteredReader[i].locationFilteredTopic->expressionLength);
            size += sizeof(filteredReader[i].locationFilteredTopic->expressionLength);

            Marshalling_enc_string(data.mValue+size, filteredReader[i].locationFilteredTopic->filterExpression, filteredReader[i].locationFilteredTopic->expressionLength);
            size += filteredReader[i].locationFilteredTopic->expressionLength;

            rc_t ret = ManagementTopicPublicationService_publishManagementDirective(&data, addr);
        }
    }

    return SDDS_RT_OK;
}


rc_t
s_key_deleteFilterExpression(DDS_char* mValue) {
    return SDDS_RT_OK;
}

static void
s_reactivateSubscription() {
    TopicDB_iteratorReset();
    while (TopicDB_iteratorHasNext()) {
        Topic_t* topic = TopicDB_iteratorNext();
        List_t* subscriptions = topic->dsinks.list;
        TopicSubscription_t* sub = subscriptions->first_fn(subscriptions);
        while (sub != NULL) {
            if (sub->state == PAUSED) {
                msec16_t remainingMSec;
                if (Time_remainMSec16(sub->updated, &remainingMSec) != SDDS_SSW_RT_OK) {
                    Log_error("Unable to get remainig time.\n");
                }

                remainingMSec = abs(remainingMSec);
                msec16_t deadline = ((SDDS_SUBSCRIPTION_REACTIVATION_TIMER_SEC * 1000) + SDDS_SUBSCRIPTION_REACTIVATION_TIMER_MSEC);
                if (remainingMSec >= deadline) {
                    sub->state = ACTIVE;
                    if (Time_getTime16(&sub->updated) != SDDS_SSW_RT_OK) {
                        Log_error("Unable to get time.\n");
                    }
                }
//                printf("Subscription (%d,%x) automatically resumed.\n", topic->id, sub->participant);
            }
            sub = subscriptions->next_fn(subscriptions);
        }
    }
}

#endif
