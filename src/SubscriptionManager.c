/*
 * SubscriptionManager.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionManager.h"
#include "ManagementTopic.h"


extern GeometryStore_t g_geometryStore;

rc_t
SubscriptionManager_init(SubscriptionManager_t* self) {
    return SDDS_RT_OK;
}

rc_t
SubscriptionManager_evalFilteredSubscription(SubscriptionManager_t* self, DeviceLocation_t* sample) {

}

rc_t
SubscriptionManager_handleParticipant(SubscriptionManager_t* self, SDDS_DCPSParticipant* sample) {

}

rc_t
SubscriptionManager_handlePupblication(SubscriptionManager_t* self, DDS_DCPSPublication* sample) {

}

rc_t
SubscriptionManager_handleSubscription(SubscriptionManager_t* self, SDDS_DCPSSubscription* sample) {

}

rc_t
SubscriptionManager_registerFilter(SubscriptionManager_t* self, SDDS_DCPSManagement* sample) {
    SDDS_DCPSManagement data;
    data.participant = SDDS_MANAGEMENT_TOPIC_ALL_PARTICIPANTS;
    strcpy(data.mKey, SDDS_MANAGEMENT_TOPIC_KEY_SEND_FILTER_EXPRESSION);

    int size = 0;

    topicid_t topicID;
    Marshalling_dec_uint8(data.mValue+size, &topicID);
    size += sizeof(topicID);

    SSW_NodeID_t participantID;

    Marshalling_dec_uint16(data.mValue+size, &participantID);
    size += sizeof(participantID);

    Topic_t* topic = TopicDB_getTopic(topicID);
    LocationFilteredTopic_t* locTopic = malloc(sizeof(LocationFilteredTopic_t));
    rc_t ret = LocationFilteredTopic_create(locTopic, topic, NULL, g_geometryStore);

    Marshalling_dec_uint8(data.mValue+size, &locTopic->expressionLength);
    size += sizeog(locTopic->expressionLength);

    Marshalling_enc_string(data.mValue+size, locTopic->filterExpression, locTopic->expressionLength);

    List_t* edges = self.subscriptionGraph.edges;
    DirectedEdge_t* edge = (DirectedEdge_t*) edges->first_fn(edges);
    while (edges != NULL) {
        if (edge->subscriber == participantID) {
            ret = SubscriptionGraph_registerFilter(edge, locTopic);
            if (ret != SDDS_RT_OK) {
                Log_error("Unable to register Filter.\n");
            }
            edge = (DirectedEdge_t*) edges->next_fn(edges);
        }
    }

    return SDDS_RT_OK;
}
