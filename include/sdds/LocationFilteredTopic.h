/*
 * LocationFilteredTopic.h
 *
 *  Created on: Aug 9, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_
#define SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_

#include "ContentFilteredTopic.h"
#include "GeometryStore.h"
#include "os-ssal/LocationService.h"
#include "sdds_types.h"

#ifndef SDDS_FILTER_EXPR_MAX_LEN
#define SDDS_FILTER_EXPR_MAX_LEN 32
#endif

enum ConnectorType {
    CONNECTOR_NONE,
    CONNECTOR_AND,
    CONNECTOR_OR
};
typedef enum ConnectorType ConnectorType_t;

struct FilterExpression {
    bool_t negation;
    uint8_t functionID;
    uint8_t geometryID;
};
typedef struct FilterExpression FilterExpression_t;

struct FilterState {
    bool_t result;
    ConnectorType_t connector;
    uint8_t currentPosition;
    FilterExpression_t expression;
};
typedef struct FilterState FilterState_t;

struct LocationFilteredTopic {
    ContentFilteredTopic_t contentFilteredTopic;
    uint8_t filterExpression[SDDS_FILTER_EXPR_MAX_LEN];
    uint8_t expressionLength;
    FilterState_t filterstate;
    GeometryStore_t geometryStore;
};
typedef struct LocationFilteredTopic LocationFilteredTopic_t;

rc_t
LocationFilteredTopic_create(LocationFilteredTopic_t* self, Topic_t* topic, char* filterExpression);

rc_t
LocationFilteredTopic_setFilter(LocationFilteredTopic_t* self, char* filterExpression);

rc_t
LocationFilteredTopic_evalSample(LocationFilteredTopic_t* self, Data data);

rc_t
LocationFilteredTopic_evalExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc);

#endif /* SDDS_INCLUDE_SDDS_LOCATIONFILTEREDTOPIC_H_ */
