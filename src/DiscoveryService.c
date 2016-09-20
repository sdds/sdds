/*
 * DiscoveryService.c
 *
 *  Created on: Aug 1, 2016
 *      Author: o_dedi
 */

#include "DiscoveryService.h"

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED

#   ifndef SDDS_DISCOVERY_MAX_PARTICIPANTS
#define SDDS_DISCOVERY_MAX_PARTICIPANTS 10
#   endif

static SDDS_DCPSParticipant participants[SDDS_DISCOVERY_MAX_PARTICIPANTS];

static void
s_DiscoveryService_participantListener(DDS_DataReader reader);
static void
s_DiscoveryService_topicListener(DDS_DataReader reader);
static void
s_DiscoveryService_publicationListener(DDS_DataReader reader);
static void
s_DiscoveryService_subscriptionListener(DDS_DataReader reader);

static rc_t
s_DiscoveryService_addParticipant(SDDS_DCPSParticipant* p);
static rc_t
s_DiscoveryService_handleParticipant(SDDS_DCPSParticipant* p);

rc_t
DiscoveryService_init() {
    DDS_ReturnCode_t dds_ret;
    struct DDS_DataReaderListener participantListStruct = {
            .on_data_available =
            &s_DiscoveryService_participantListener
    };
    dds_ret = DDS_DataReader_set_listener(g_DCPSParticipant_reader, &participantListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set participantListener.\n");
        return SDDS_RT_FAIL;
    }

    struct DDS_DataReaderListener topicListStruct = {
            .on_data_available =
            &s_DiscoveryService_topicListener
    };
    dds_ret = DDS_DataReader_set_listener(g_DCPSTopic_reader, &topicListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set topicListener.\n");
        return SDDS_RT_FAIL;
    }

    struct DDS_DataReaderListener publicationListStruct = {
            .on_data_available =
            &s_DiscoveryService_publicationListener
    };
    dds_ret = DDS_DataReader_set_listener(g_DCPSPublication_reader, &publicationListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set publicationListener.\n");
        return SDDS_RT_FAIL;
    }

    struct DDS_DataReaderListener subscriptionListStruct = {
            .on_data_available =
            &s_DiscoveryService_subscriptionListener
    };
    dds_ret = DDS_DataReader_set_listener(g_DCPSSubscription_reader, &subscriptionListStruct, NULL);
    if(dds_ret == DDS_RETCODE_ERROR){
        Log_error("Unable to set subscriptionListener.\n");
        return SDDS_RT_FAIL;
    }

    return SDDS_RT_OK;
}

static void
s_DiscoveryService_participantListener(DDS_DataReader reader) {
    DDS_ReturnCode_t ret;
    SDDS_DCPSParticipant p_data_used;
    SDDS_DCPSParticipant* p_data_used_ptr = &p_data_used;

    do {
        ret = DDS_DCPSParticipantDataReader_take_next_sample(reader, (DDS_DCPSParticipant**) &p_data_used_ptr, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
#   ifdef UTILS_DEBUG
            Log_info("no participant data\n");
#   endif
        }
        else {
            Log_info("Received (participant):[%x]\n", p_data_used.data.key);
            if (s_DiscoveryService_handleParticipant(&p_data_used) != SDDS_RT_OK) {
                Log_error("Error in s_DiscoveryService_handleParticipant\n.");
            }

#       ifdef FEATURE_SDDS_SUBSCRIPTION_MANAGER_ENABLED
            rc_t retSM;
            retSM = SubscriptionManagementService_handleParticipant(&p_data_used);
            if (retSM != SDDS_RT_OK) {
                Log_error("SubscriptionManagementService_handleParticipant failed.\n");
            }
#       endif
        }
    } while ((ret != DDS_RETCODE_NO_DATA) && (ret != DDS_RETCODE_ERROR));
}

static void
s_DiscoveryService_topicListener(DDS_DataReader reader) {
    // To Do
}

static void
s_DiscoveryService_publicationListener(DDS_DataReader reader) {
#   if defined(SDDS_TOPIC_HAS_PUB) || defined(SDDS_SUBSCRIPTION_MANAGER)
    rc_t ret;
    DDS_DCPSPublication pt_data_used;
    DDS_DCPSPublication* pt_data_used_ptr = &pt_data_used;

    topicid_t tIDs[SDDS_DISCOVERY_MAX_PARTICIPANTS];
    memset(tIDs, 0, SDDS_DISCOVERY_MAX_PARTICIPANTS);
    bool sub = true;
    int nextID = 0;

    do {
        ret = DDS_DCPSPublicationDataReader_take_next_sample(reader, &pt_data_used_ptr, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
#       ifdef UTILS_DEBUG
            Log_info("no publication data\n");
#       endif
        }
        else {
            Log_info("Received (publication):[%u][%x] topic:%u\n",
                      pt_data_used.key, pt_data_used.participant_key,
                      pt_data_used.topic_id);
            for (int i = 0; i <= nextID; i++) {
                if (tIDs[i] == pt_data_used.topic_id) {
                    sub = false;
                    break;
                }
            }
            if (sub) {
                tIDs[nextID] = pt_data_used.topic_id;
                nextID = (nextID + 1) % SDDS_DISCOVERY_MAX_PARTICIPANTS;
                BuiltInTopicPublicationService_publishDCPSSubscription(pt_data_used.topic_id);
            }

#       ifdef SDDS_SUBSCRIPTION_MANAGER
            rc_t retSM;
            retSM = SubscriptionManagementService_handlePublication(&pt_data_used);
            if (retSM != SDDS_RT_OK) {
                Log_error("SubscriptionManagementService_handlePublication failed.\n");
            }
#       endif
        }
    } while ((ret != DDS_RETCODE_NO_DATA) && (ret != DDS_RETCODE_ERROR));
#   endif
}

static void
s_DiscoveryService_subscriptionListener(DDS_DataReader reader) {
#   if defined(SDDS_TOPIC_HAS_SUB) || defined(SDDS_SUBSCRIPTION_MANAGER)
    SDDS_DCPSSubscription st_data_used;
    SDDS_DCPSSubscription* st_data_used_ptr = &st_data_used;
    rc_t ret;

    do {
        ret = DDS_DCPSSubscriptionDataReader_take_next_sample(reader, (DDS_DCPSSubscription**) &st_data_used_ptr, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
#       ifdef UTILS_DEBUG
            Log_info("no subscription data\n");
#       endif
        }
        else {
            Log_info("Received (subscription):[%u][%x] topic:%u\n",
                      st_data_used.data.key, st_data_used.data.participant_key,
                      st_data_used.data.topic_id);
            Topic_t* topic = NULL;
            ret = DataSource_getTopic(NULL, st_data_used.data.topic_id, &topic);
            if (ret == SDDS_RT_OK) {
                Locator_upRef(st_data_used.addr);
                if (Topic_addRemoteDataSink(topic, st_data_used.addr, st_data_used.data.participant_key, ACTIVE) != SDDS_RT_OK) {
                    Locator_downRef(st_data_used.addr);
                }
            }

#       ifdef SDDS_SUBSCRIPTION_MANAGER
            rc_t retSM;
            retSM = SubscriptionManagementService_handleSubscription(&st_data_used);
            if (retSM != SDDS_RT_OK) {
                Log_error("SubscriptionManagementService_handleSubscription failed.\n");
            }
#       endif
        }
    } while ((ret != DDS_RETCODE_NO_DATA) && (ret != DDS_RETCODE_ERROR));
#   endif
}

static rc_t
s_DiscoveryService_addParticipant(SDDS_DCPSParticipant* p) {
    int i;
    int freePos = -1;

    if (BuiltinTopic_participantID == p->data.key) {
        return SDDS_RT_KNOWN;
    }

    for (i = 0; i < SDDS_DISCOVERY_MAX_PARTICIPANTS; i++) {
        if (participants[i].data.key == p->data.key) {
            return SDDS_RT_KNOWN;
        }
        else if ((participants[i].data.key == 0) && (freePos == -1)) {
            freePos = i;
        }
    }

    if (freePos != -1) {
        memcpy(&participants[freePos], p, sizeof(SDDS_DCPSParticipant));
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}

static rc_t
s_DiscoveryService_handleParticipant(SDDS_DCPSParticipant* p) {
    rc_t ret = s_DiscoveryService_addParticipant(p);

    if (ret == SDDS_RT_OK) {
#   if defined(SDDS_TOPIC_HAS_PUB)
        ret = Topic_addRemoteDataSink(g_DCPSSubscription_topic, p->addr, p->data.key, ACTIVE);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(p->addr);
            return ret;
        }
#   endif

#   if defined(SDDS_TOPIC_HAS_SUB)
        ret = Topic_addRemoteDataSink(g_DCPSPublication_topic, p->addr, p->data.key, ACTIVE);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(p->addr);
            return ret;
        }
        BuiltInTopicPublicationService_publishDCPSPublication();
#   endif

        BuiltInTopicPublicationService_publishDCPSParticipant();
        return SDDS_RT_OK;
    }
    Locator_downRef(p->addr);
    return SDDS_RT_OK;
}

#endif
