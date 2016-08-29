/*
 * SubscriptionGraph.h
 *
 *  Created on: Aug 28, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_

#include "sDDS.h"
#include "LocationFilteredTopic.h"
#include "List.h"
#include <stdint.h>

enum PropagationType {
    SDDS_PROPAGATION_MULTICAST,
    SDDS_PROPAGATION_UNICAST
};
typedef enum PropagationType PropagationType_t;

struct ParticipantNode {
    uint8_t roleMask;
    DeviceLocation_t devLoc;
    Locator_t* addr;
    List_t* edges;
};
typedef struct ParticipantNode ParticipantNode_t;

struct DirectedEdge {
    bool_t filteredSubscription;
    void* topic;
    ParticipantNode_t* publisher;
    ParticipantNode_t* subscriber;
};
typedef struct DirectedEdge DirectedEdge_t;

struct SubscriptionGraph {
    List_t* edges;
    List_t* nodes;
};
typedef struct SubscriptionGraph SubscriptionGraph_t;

List_t*
ParticipantNode_getEdges();

rc_t
SubscriptionGraph_init(SubscriptionGraph_t *self);

ParticipantNode_t*
SubscriptionGraph_createParticipantNode();

DirectedEdge_t*
SubscriptionGraph_establishSubscription(ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic);

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic);

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic);

rc_t
SubscriptionGraph_cancelSubscription(DirectedEdge_t* edge);

rc_t
SubscriptionGraph_pauseSubscription(DirectedEdge_t* edge);

rc_t
SubscriptionGraph_resumeSubscription(DirectedEdge_t* edge);

#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_ */
