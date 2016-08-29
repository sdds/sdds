/*
 * SubscriptionGraph.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionGraph.h"

List_t*
ParticipantNode_getEdges() {

}

rc_t
SubscriptionGraph_init(SubscriptionGraph_t* self) {

}

ParticipantNode_t*
SubscriptionGraph_createParticipantNode() {

}

DirectedEdge_t*
SubscriptionGraph_establishSubscription(ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic) {

}

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic) {

}

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic) {

}

rc_t
SubscriptionGraph_cancelSubscription(DirectedEdge_t* edge) {

}

rc_t
SubscriptionGraph_pauseSubscription(DirectedEdge_t* edge) {

}

rc_t
SubscriptionGraph_resumeSubscription(DirectedEdge_t* edge) {

}
