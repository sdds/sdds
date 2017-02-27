/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      SubscriptionGraph.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_

#include "sDDS.h"
#include "LocationFilteredTopic.h"
#include "List.h"
#include <stdint.h>

#define SDDS_PARTICIPANT_ROLE_SUB       (1 << 0)
#define SDDS_PARTICIPANT_ROLE_PUB       (1 << 1)
#define SDDS_PARTICIPANT_ROLE_SUB_PUB   (SDDS_PARTICIPANT_ROLE_SUB | SDDS_PARTICIPANT_ROLE_PUB)

enum PropagationType {
    SDDS_PROPAGATION_MULTICAST,
    SDDS_PROPAGATION_UNICAST
};
typedef enum PropagationType PropagationType_t;

struct ParticipantNode {
    SSW_NodeID_t id;
    uint8_t roleMask;
    DeviceLocation_t devLoc;
    Locator_t* addr;
    List_t* edges;
    List_t* topics;
};
typedef struct ParticipantNode ParticipantNode_t;

struct DirectedEdge {
    bool_t filteredSubscription;
    Topic_t* topic;
    List_t* locTopics;
    ParticipantNode_t* publisher;
    ParticipantNode_t* subscriber;
    SubscriptionState_t state;
};
typedef struct DirectedEdge DirectedEdge_t;

struct SubscriptionGraph {
    List_t* edges;
    List_t* nodes;
};
typedef struct SubscriptionGraph SubscriptionGraph_t;

List_t*
ParticipantNode_getEdges(ParticipantNode_t* node);

rc_t
SubscriptionGraph_init(SubscriptionGraph_t *self);

ParticipantNode_t*
SubscriptionGraph_createParticipantNode(SubscriptionGraph_t *self);

rc_t
SubscriptionGraph_freeParticipantNode(SubscriptionGraph_t *self, ParticipantNode_t* node);

DirectedEdge_t*
SubscriptionGraph_createDirectedEdge(SubscriptionGraph_t *self);

rc_t
SubscriptionGraph_freeDirectedEdge(SubscriptionGraph_t *self, DirectedEdge_t*edge);

DirectedEdge_t*
SubscriptionGraph_establishSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, Topic_t* topic);

DirectedEdge_t*
SubscriptionGraph_establishFilteredSubscription(SubscriptionGraph_t *self, ParticipantNode_t* pub, ParticipantNode_t* sub, LocationFilteredTopic_t* topic);

rc_t
SubscriptionGraph_registerFilter(DirectedEdge_t* edge, LocationFilteredTopic_t* topic);

rc_t
SubscriptionGraph_cancelSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge);

rc_t
SubscriptionGraph_pauseSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge);

rc_t
SubscriptionGraph_resumeSubscription(SubscriptionGraph_t *self, DirectedEdge_t* edge);

ParticipantNode_t*
SubscriptionGraph_containsParticipantNode(SubscriptionGraph_t *self, SSW_NodeID_t participantID);

DirectedEdge_t*
SubscriptionGraph_containsSubscription(SubscriptionGraph_t *self, SSW_NodeID_t pubID, SSW_NodeID_t subID, topicid_t topicID);

#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONGRAPH_H_ */
