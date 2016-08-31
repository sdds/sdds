/*
 * SubscriptionGraph.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionGraph.h"

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

}

rc_t
SubscriptionGraph_freeParticipantNode(SubscriptionGraph_t *self) {

}

DirectedEdge_t*
SubscriptionGraph_establishSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic) {

}

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic) {

}

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic) {

}

rc_t
SubscriptionGraph_cancelSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {

}

rc_t
SubscriptionGraph_pauseSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {

}

rc_t
SubscriptionGraph_resumeSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge) {

}

ParticipantNode_t*
SubscriptionGraph_containsParticipantNode(SubscriptionGraph_t *self, ParticipantNode_t* node) {

}

DirectedEdge_t*
SubscriptionGraph_containsSubscription(SubscriptionGraph_t *self, SSW_NodeID_t pubID, SSW_NodeID_t subID, topicid_t topicID) {

}
