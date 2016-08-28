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
#include "DirectedEdgeList.h"
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
    DirectedEdgeList_t edges;
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

};
typedef struct SubscriptionGraph SubscriptionGraph_t;

DirectedEdgeList_t*
ParticipantNode_getEdges();

#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_ */
