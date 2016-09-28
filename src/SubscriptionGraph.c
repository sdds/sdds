/*
 * SubscriptionGraph.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionGraph.h"

#ifdef FEATURE_SDDS_SUBSCRIPTION_MANAGER_ENABLED

#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
time32_t start_time;
#endif

List_t*
ParticipantNode_getEdges(ParticipantNode_t* node) {
    return node->edges;
}

rc_t
SubscriptionGraph_init(SubscriptionGraph_t* self) {
    self->nodes = List_initDynamicLinkedList();
    self->edges = List_initDynamicLinkedList();
    return SDDS_RT_OK;
}

ParticipantNode_t*
SubscriptionGraph_createParticipantNode(SubscriptionGraph_t *self) {
    ParticipantNode_t* node = malloc(sizeof(ParticipantNode_t));
    node->topics = List_initDynamicLinkedList();
    node->edges = List_initDynamicLinkedList();
    node->roleMask = 0;
    List_t* nodes = self->nodes;

#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
    if (nodes->size_fn(nodes) == 0) {
        Time_getTime32(&start_time);
    }
#endif

    rc_t ret = nodes->add_fn(nodes, node);
    if (ret != SDDS_RT_OK) {
        free(node->topics);
        free(node->edges);
        free(node);
        return NULL;
    }
    return node;
}

rc_t
SubscriptionGraph_freeParticipantNode(SubscriptionGraph_t *self, ParticipantNode_t* node) {
    List_t* edges = self->edges;
    DirectedEdge_t* edge = edges->first_fn(edges);
    while (edge != NULL) {
        if ((edge->publisher == node) || (edge->subscriber == node)) {
            rc_t ret = edges->delete_fn(edges);
            if (ret != SDDS_RT_OK) {
                return SDDS_RT_FAIL;
            }
            ret = SubscriptionGraph_freeDirectedEdge(self, edge);
            if (ret != SDDS_RT_OK) {
                return SDDS_RT_FAIL;
            }
        }

        edge = edges->next_fn(edges);
    }

    List_t* nodes = self->nodes;
    ParticipantNode_t* n = nodes->first_fn(nodes);
    while (n != NULL) {
        if (n == node) {
            nodes->delete_fn(nodes);
        }
        n = nodes->next_fn(nodes);
    }

    rc_t ret = nodes->delete_all_fn(nodes);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    ret = node->edges->delete_all_fn(node->edges);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    free(node->topics);
    free(node->edges);
    free(node);

    return SDDS_RT_OK;
}

DirectedEdge_t*
SubscriptionGraph_createDirectedEdge(SubscriptionGraph_t *self) {
    DirectedEdge_t* edge = malloc(sizeof(DirectedEdge_t));
    edge->locTopics = List_initDynamicLinkedList();
    List_t* edges = self->edges;
    rc_t ret = edges->add_fn(edges, edge);
    if (ret != SDDS_RT_OK) {
        free(edge->locTopics);
        free(edge);
        return NULL;
    }

#ifdef SDDS_EVAL_SUBSCRIPTION_GRAPH
    if (edges->size_fn(edges) == SDDS_EVAL_SUBSCRIPTION_GRAPH_MAX_EDGES) {
        msec32_t build_time;
        Time_remainMSec32(start_time, &build_time);

        time32_t now;
        Time_getTime32(&now);
    
        printf("Subscrption Graph Build (now, start, duration):\n");
        printf("%u, %u, %d\n", now, start_time, abs(build_time));
    }
#endif
    return edge;
}

rc_t
SubscriptionGraph_freeDirectedEdge(SubscriptionGraph_t *self, DirectedEdge_t*edge) {
    List_t* nodes = self->nodes;
    ParticipantNode_t* node = nodes->first_fn(nodes);
    while (nodes != NULL) {
        List_t* nodeEdges = node->edges;
        DirectedEdge_t* nodeEdge = nodeEdges->first_fn(nodeEdges);
        while (nodeEdge != NULL) {
            if (nodeEdge == edge) {
                rc_t ret = nodeEdges->delete_fn(nodeEdges);
                if (ret != SDDS_RT_OK) {
                    return SDDS_RT_FAIL;
                }
            }
            nodeEdge = nodeEdges->next_fn(nodeEdges);
        }
        node = nodes->next_fn(nodes);
    }

    List_t* edges = self->edges;
    DirectedEdge_t* e = edges->first_fn(edges);
    while (e != NULL) {
        if (e == edge) {
            rc_t ret = edges->delete_fn(edges);
            if (ret != SDDS_RT_OK) {
                return SDDS_RT_FAIL;
            }
        }
        e = edges->next_fn(edges);
    }

    rc_t ret = edge->locTopics->delete_all_fn(edge->locTopics);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    free(edge->locTopics);
    free(edge);
    return SDDS_RT_OK;

}

DirectedEdge_t*
SubscriptionGraph_establishSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic) {
    assert(self);
    assert(pub);
    assert(sub);
    assert(topic);

    pub->roleMask |= SDDS_PARTICIPANT_ROLE_PUB;
    sub->roleMask |= SDDS_PARTICIPANT_ROLE_SUB;
    DirectedEdge_t* edge = SubscriptionGraph_createDirectedEdge(self);
    assert(edge);

    edge->publisher = pub;
    edge->subscriber = sub;
    edge->topic = topic;
    edge->filteredSubscription = false;

    return edge;
}

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic) {
    assert(self);
    assert(pub);
    assert(sub);
    assert(topic);

    pub->roleMask |= SDDS_PARTICIPANT_ROLE_PUB;
    sub->roleMask |= SDDS_PARTICIPANT_ROLE_SUB;
    DirectedEdge_t* edge = SubscriptionGraph_createDirectedEdge(self);
    assert(edge);

    edge->publisher = pub;
    edge->subscriber = sub;
    edge->topic = topic->contentFilteredTopic.associatedTopic;
    rc_t ret = edge->locTopics->add_fn(edge->locTopics, topic);
    if (ret != SDDS_RT_OK) {
        return NULL;
    }
    edge->filteredSubscription = true;

    return edge;
}

rc_t
s_containsLocationFilteredTopic(DirectedEdge_t* edge, LocationFilteredTopic_t* topic) {
    assert(edge);
    assert(topic);

    List_t* locTopics = edge->locTopics;
    LocationFilteredTopic_t* lt = locTopics->first_fn(locTopics);
    while (lt != NULL) {
        if (LocationFilteredTopic_equals(lt, topic) == SDDS_RT_OK) {
            //printf("Filter known\n");
            return SDDS_RT_OK;
        }
        lt = locTopics->next_fn(locTopics);
    }

    return SDDS_RT_FAIL;
}

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic) {
    assert(edge);
    assert(topic);
    assert(edge->topic == topic->contentFilteredTopic.associatedTopic);

    List_t* locTopics = edge->locTopics;

    if (s_containsLocationFilteredTopic(edge, topic) == SDDS_RT_OK) {
        return SDDS_RT_OK;
    }

    rc_t ret = locTopics->add_fn(locTopics, topic);
    edge->filteredSubscription = true;

    return SDDS_RT_OK;
}

rc_t
SubscriptionGraph_cancelSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {

    return SubscriptionGraph_freeDirectedEdge(self, edge);
}

rc_t
SubscriptionGraph_pauseSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {
    edge->state = PAUSED;
    return SDDS_RT_OK;
}

rc_t
SubscriptionGraph_resumeSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {
    edge->state = ACTIVE;
    return SDDS_RT_OK;
}

ParticipantNode_t*
SubscriptionGraph_containsParticipantNode(SubscriptionGraph_t *self, SSW_NodeID_t participantID) {
    List_t* nodes = self->nodes;
    ParticipantNode_t* node = nodes->first_fn(nodes);
    while(node != NULL) {
        if  (node->id == participantID) {
            break;
        }
        node = nodes->next_fn(nodes);
    }

    return node;
}

DirectedEdge_t*
SubscriptionGraph_containsSubscription(SubscriptionGraph_t *self, SSW_NodeID_t pubID, SSW_NodeID_t subID, topicid_t topicID) {
    List_t* edges = self->edges;
    DirectedEdge_t* edge = edges->first_fn(edges);
    while (edge != NULL) {
        if ((edge->publisher->id == pubID) && (edge->subscriber->id == subID) && (edge->topic->id == topicID)) {
            return edge;
        }
        edge = edges->next_fn(edges);
    }

    return NULL;
}

#endif
