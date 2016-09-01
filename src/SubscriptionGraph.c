/*
 * SubscriptionGraph.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionGraph.h"

#ifdef FEATURE_SDDS_SUBSCRIPION_MANAGER_ENABLED

List_t*
ParticipantNode_getEdges(ParticipantNode_t* node) {
    return node->edges;
}

rc_t
SubscriptionGraph_init(SubscriptionGraph_t* self) {
    return SDDS_RT_OK;
}

ParticipantNode_t*
SubscriptionGraph_createParticipantNode(SubscriptionGraph_t *self) {
    ParticipantNode_t* node = calloc(1, sizeof(ParticipantNode_t*));
    node->topics = List_initDynamicLinkedList();
    node->edges = List_initDynamicLinkedList();
    List_t* nodes = self->nodes;
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
    DirectedEdge_t* edge = calloc(1, sizeof(DirectedEdge_t*));
    List_t* edges = self->edges;
    rc_t ret = edges->add_fn(edges, edge);
    if (ret != SDDS_RT_OK) {
        free(edge);
        return NULL;
    }
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

    free(edge);
    return SDDS_RT_OK;

}

DirectedEdge_t*
SubscriptionGraph_establishSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic) {
    DirectedEdge_t* edge = SubscriptionGraph_createDirectedEdge(self);
    if (edge != NULL) {
        edge->publisher = pub;
        edge->subscriber = sub;
        edge->topic = topic;
        edge->filteredSubscription = false;
    }
    return edge;
}

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic) {
    DirectedEdge_t* edge = SubscriptionGraph_createDirectedEdge(self);
    if (edge != NULL) {
        edge->publisher = pub;
        edge->subscriber = sub;
        edge->topic = topic->contentFilteredTopic.associatedTopic;
        edge->locTopic = topic;
        edge->filteredSubscription = true;
    }
    return edge;
}

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic) {
    assert(edge);
    assert(topic);

    edge->filteredSubscription = true;
    edge->topic = topic->contentFilteredTopic.associatedTopic;
    edge->locTopic = topic;

    return SDDS_RT_OK;
}

rc_t
SubscriptionGraph_cancelSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {

    return SubscriptionGraph_freeDirectedEdge(self, edge);
}

rc_t
SubscriptionGraph_pauseSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {
    edge->sate = PAUSED;
    return SDDS_RT_OK;
}

rc_t
SubscriptionGraph_resumeSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {
    edge->sate = ACTIVE;
    return SDDS_RT_OK;
}

ParticipantNode_t*
SubscriptionGraph_containsParticipantNode(SubscriptionGraph_t *self, SSW_NodeID_t participantID) {
    List_t* nodes = self->nodes;
    ParticipantNode_t* node = nodes->first_fn(nodes);
    while((node != NULL) && (node->id != participantID)) {
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
