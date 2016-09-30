/*
 * SubscriptionManager.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionManager.h"
#include "ManagementTopic.h"
#include "Locator.h"
#include "FilterEvaluator.h"
#include "ManagementTopicPublicationService.h"

#ifdef FEATURE_SDDS_SUBSCRIPTION_MANAGER_ENABLED

#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
time32_t start_time;
#endif

extern GeometryStore_t g_geometryStore;

rc_t
SubscriptionManager_init(SubscriptionManager_t* self) {
#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
        Time_getTime32(&start_time);
#endif
    return SubscriptionGraph_init(&self->subscriptionGraph);
}

rc_t
SubscriptionManager_evalFilteredSubscription(SubscriptionManager_t* self, DeviceLocation_t* sample) {
    List_t* edges = self->subscriptionGraph.edges;
    DirectedEdge_t* edge = (DirectedEdge_t*) edges->first_fn(edges);

    while (edge != NULL) {
        if (sample->device == edge->publisher->id) {
            SubscriptionState_t prevState = edge->state;

            rc_t ret = FilterEvaluator_evalSubscription(sample, edge);
            if ((ret == SDDS_RT_FAIL)) {
                ret = SubscriptionGraph_pauseSubscription(&self->subscriptionGraph, edge);
                if (ret != SDDS_RT_OK) {
                    Log_error("Unable to pause subscription.\n");
                    edge = (DirectedEdge_t*) edges->next_fn(edges);
                    continue;
                }
            }
            else {
                ret = SubscriptionGraph_resumeSubscription(&self->subscriptionGraph, edge);
                if (ret != SDDS_RT_OK) {
                    Log_error("Unable to resume subscription.\n");
                    edge = (DirectedEdge_t*) edges->next_fn(edges);
                    continue;
                }
            }

            if (prevState != edge->state) {
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
                Log_info("set Subscription State: ");
                switch (edge->state) {
                case CANCELLED:
                    Log_info("CANCELLED\n");
                    break;
                case ACTIVE:
                    Log_info("ACTIVE\n");
                    break;
                case PAUSED:
                    Log_info("PAUSED\n");
                    break;
                default:
                    Log_info("UNKNOWN\n");
                    break;
                }
#endif
                ret = SubscriptionManager_publishSubscriptionState(edge);
                if (ret != SDDS_RT_OK) {
                    Log_error("Unable to publish Subscription state.\n");
                    edge = (DirectedEdge_t*) edges->next_fn(edges);
                    continue;
                }
            }
        }
        edge = (DirectedEdge_t*) edges->next_fn(edges);
    }

    return SDDS_RT_OK;
}

rc_t
SubscriptionManager_publishSubscriptionState(DirectedEdge_t* edge) {
    assert(edge);

    rc_t ret;
    SDDS_DCPSManagement data;
    data.participant = edge->publisher->id;
    strcpy(data.mKey, SDDS_MANAGEMENT_TOPIC_KEY_SET_SUBSCRIPTION_STATE);
    int size = 0;
    ret =  Marshalling_enc_uint8((byte_t*) data.mValue + size, &edge->topic->id);
    size += sizeof(edge->topic->id);
    ret =  Marshalling_enc_uint16((byte_t*) data.mValue + size, &edge->subscriber->id);
    size += sizeof(edge->publisher->id);
    ret =  Marshalling_enc_uint8((byte_t*) data.mValue + size, (uint8_t*)&edge->state);

    return ManagementTopicPublicationService_publishManagementDirective(&data, edge->publisher->addr);
}

rc_t
SubscriptionManager_handleParticipant(SubscriptionManager_t* self, SDDS_DCPSParticipant* sample) {
    ParticipantNode_t* node = SubscriptionGraph_containsParticipantNode(&self->subscriptionGraph, sample->data.key);
    if (node == NULL) {
        ParticipantNode_t* node = SubscriptionGraph_createParticipantNode(&self->subscriptionGraph);

        Locator_upRef(sample->srcAddr);
        node->addr = sample->srcAddr;
        node->id = sample->data.key;

        List_t* nodes = self->subscriptionGraph.nodes;
        rc_t ret = nodes->add_fn(nodes, node);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(sample->srcAddr);
            Log_error("Unable to add new participant node.\n");
            SubscriptionGraph_freeParticipantNode(&self->subscriptionGraph, node);
            return SDDS_RT_FAIL;
        }
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
        Log_info("Create node %x\n", node->id);
#endif
    }
    else {
     //   printf("node %x exists\n", sample->data.key);
    }

    return SDDS_RT_OK;
}

rc_t
SubscriptionManager_handlePublication(SubscriptionManager_t* self, DDS_DCPSPublication* sample) {
    SDDS_DCPSParticipant pt;
    pt.participantID = sample->participant_key;
    pt.srcAddr = sample->srcAddr;

    if (SubscriptionManager_handleParticipant(self, &pt) != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    ParticipantNode_t* node = SubscriptionGraph_containsParticipantNode(&self->subscriptionGraph, sample->participant_key);

    if (node != NULL) {
        node->roleMask |= SDDS_PARTICIPANT_ROLE_PUB;

        List_t* topics = node->topics;
        Topic_t* topic = (Topic_t*) topics->first_fn(topics);
        while (topic != NULL) {
            if (topic->id == sample->topic_id) {
                break;
            }
            topic = (Topic_t*) topics->next_fn(topics);
        }

        if (topic == NULL) {
            Topic_t* t = TopicDB_getTopic(sample->topic_id);
            if (t == NULL) {
                Log_error("Unknown topic: %d.\n", sample->topic_id);
                return SDDS_RT_FAIL;
            }
            rc_t ret = topics->add_fn(topics, TopicDB_getTopic(sample->topic_id));
            if (ret != SDDS_RT_OK) {
                Log_error("Unable to add topic to publisher.\n");
                return ret;
            }
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
            Log_info("node %x is publisher for topic %d\n", node->id, sample->topic_id);
#endif
        }
    }
    else {
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
        Log_info("Unknown node %x\n", sample->participant_key);
#endif
    }

    return SDDS_RT_OK;
}

rc_t
SubscriptionManager_handleSubscription(SubscriptionManager_t* self, DDS_DCPSSubscription* sample) {
    ParticipantNode_t* sub = SubscriptionGraph_containsParticipantNode(&self->subscriptionGraph, sample->participant_key);

    if (sub != NULL)  {
        List_t* nodes = self->subscriptionGraph.nodes;
        ParticipantNode_t* node = nodes->first_fn(nodes);
        while (node != NULL) {
            if (node->roleMask & SDDS_PARTICIPANT_ROLE_PUB) {
                DirectedEdge_t* edge = SubscriptionGraph_containsSubscription(&self->subscriptionGraph, node->id, sample->participant_key, sample->topic_id);
                if (edge == NULL) {
                    Topic_t* topic = TopicDB_getTopic(sample->topic_id);
                    if (topic == NULL) {
                        return SDDS_RT_FAIL;
                    }
                    edge = SubscriptionGraph_establishSubscription(&self->subscriptionGraph, node, sub, topic);
                    if (edge == NULL) {
                        return SDDS_RT_FAIL;
                    }
                    
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
                    Log_info("New subscription: {%x --(%d)--> %x}\n", node->id, topic->id, sub->id);
                    Locator_print(edge->publisher->addr);
                    Locator_print(edge->subscriber->addr);
#endif
#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
                    if (self->subscriptionGraph.edges->size_fn(self->subscriptionGraph.edges) == SDDS_EVAL_SUBSCRIPTION_GRAPH_MAX_EDGES) {
                        msec32_t build_time;
                        Time_remainMSec32(start_time, &build_time);

                        time32_t now;
                        Time_getTime32(&now);
    
                        printf("Subscrption Graph Build (now, start, duration):\n");
                        printf("%u, %u, %d\n", now, start_time, abs(build_time));
                    }
#endif
                    
                }
            }
            node = nodes->next_fn(nodes);
        }

        assert(sub->addr);

        SDDS_DCPSManagement data;
        data.participant = sub->id;
        strcpy(data.mKey, SDDS_MANAGEMENT_TOPIC_KEY_REQUEST_FILTER_EXPRESSION);
        rc_t ret =  Marshalling_enc_uint8((byte_t*) data.mValue, (uint8_t*)&sample->topic_id);
        if (ret != SDDS_RT_OK) {
            Log_error("Marshalling_dec_uint8 failed\n");
        }
        ret = ManagementTopicPublicationService_publishManagementDirective(&data, sub->addr);
        if (ret != SDDS_RT_OK) {
            Log_error("publish SDDS_MANAGEMENT_TOPIC_KEY_REQUEST_FILTER_EXPRESSION failed\n");
        }
    }
    else {
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
        Log_info("Unknown node\n");
#endif
    }

    return SDDS_RT_OK;
}

rc_t
SubscriptionManager_registerFilter(SubscriptionManager_t* self, SDDS_DCPSManagement* sample) {
    int size = 0;

    topicid_t topicID = 0;
    Marshalling_dec_uint8(sample->mValue+size, &topicID);
    size += sizeof(topicID);

    SSW_NodeID_t participantID;
    Marshalling_dec_uint16(sample->mValue+size, &participantID);
    size += sizeof(participantID);

    Topic_t* topic = TopicDB_getTopic(topicID);
    if (topic == NULL) {
        Log_error("Unknown topic %d\n", topicID);
        return SDDS_RT_FAIL;
    }
    LocationFilteredTopic_t* locTopic = malloc(sizeof(LocationFilteredTopic_t));
    assert(locTopic);
    rc_t ret = LocationFilteredTopic_create(locTopic, topic, NULL, &g_geometryStore);

    Marshalling_dec_uint8(sample->mValue+size, &locTopic->expressionLength);
    size += sizeof(locTopic->expressionLength);

    Marshalling_dec_string(sample->mValue+size, locTopic->filterExpression, locTopic->expressionLength);

    List_t* edges = self->subscriptionGraph.edges;
    DirectedEdge_t* edge = (DirectedEdge_t*) edges->first_fn(edges);
    while (edge != NULL) {
        if ((edge->subscriber->id == participantID) && (edge->topic->id == topic->id)) {
            ret = SubscriptionGraph_registerFilter(edge, locTopic);
            if (ret == SDDS_RT_FAIL) {
                Log_error("Unable to register Filter.\n");
            }

            if (ret == SDDS_RT_OK) {
#ifndef SDDS_EVAL_SUBSCRIPTION_GRAPH
                Log_info("Register Filter: %x\n", participantID);
#endif
            }
        }
        edge = (DirectedEdge_t*) edges->next_fn(edges);
    }

    return SDDS_RT_OK;
}

#endif
