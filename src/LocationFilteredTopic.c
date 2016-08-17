/*
 * LocationFilteredTopic.c
 *
 *  Created on: Aug 10, 2016
 *      Author: o_dedi
 */

#include "LocationFilteredTopic.h"
#include "os-ssal/LocationService.h"
#include "string.h"

#define EXPR_FUNC_EQUALS_STR        "EQUALS"
#define EXPR_FUNC_DISJOINT_STR      "DISJOINT"
#define EXPR_FUNC_INTERSECTS_STR    "INTERSECTS"
#define EXPR_FUNC_TOUCHES_STR       "TOUCHES"
#define EXPR_FUNC_CROSSES_STR       "CROSSES"
#define EXPR_FUNC_WITHIN_STR        "WITHIN"
#define EXPR_FUNC_CONTAINS_STR      "CONTAINS"
#define EXPR_FUNC_OVERLAPS_STR      "OVERLAPS"
#define EXPR_CONN_AND_STR           "AND"
#define EXPR_CONN_OR_STR            "OR"
#define EXPR_NOT_STR                "NOT"

#define EXPR_FUNC_EQUALS_VAL        1
#define EXPR_FUNC_DISJOINT_VAL      2
#define EXPR_FUNC_INTERSECTS_VAL    3
#define EXPR_FUNC_TOUCHES_VAL       4
#define EXPR_FUNC_CROSSES_VAL       5
#define EXPR_FUNC_WITHIN_VAL        6
#define EXPR_FUNC_CONTAINS_VAL      7
#define EXPR_FUNC_OVERLAPS_VAL      8

#define EXPR_CONN_AND_VAL           9
#define EXPR_CONN_OR_VAL            10

#define EXPR_NOT_VAL                11

#define EXPR_WORD_MAX_LEN           16

static char*
s_readWord(char* filterExpr, char* word);

static rc_t
s_encodeWord(LocationFilteredTopic_t* self, char* word);

static rc_t
s_readFunction(LocationFilteredTopic_t* self);

static rc_t
s_readNotFunction(LocationFilteredTopic_t* self);

static rc_t
s_readGeometry(LocationFilteredTopic_t* self);

static rc_t
s_processExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc);

static rc_t
s_readConnector(LocationFilteredTopic_t* self);

rc_t
LocationFilteredTopic_create(LocationFilteredTopic_t* self, Topic_t* topic, char* filterExpression) {
    self->contentFilteredTopic->associatedTopic = topic;
    return LocationFilteredTopic_setFilter(self, filterExpression);
}

rc_t
LocationFilteredTopic_setFilter(LocationFilteredTopic_t* self, char* filterExpression) {
    assert(self);
    assert(filterExpression);

    memset(self->filterExpression, 0, SDDS_FILTER_EXPR_MAX_LEN);
    char word[EXPR_WORD_MAX_LEN];
    char* pos = s_readWord(filterExpression, word);
    while (pos != NULL) {
        rc_t ret = s_encodeWord(self, word);
        if (ret != SDDS_RT_OK) {
            Log_error("Filter expression too long.\n");
            return SDDS_RT_FAIL;
        }
        pos = s_readWord(pos, word);
    }
    return SDDS_RT_OK;
}

rc_t
LocationFilteredTopic_evalSample(LocationFilteredTopic_t* self, Data data) {
    SSW_NodeID_t* device = (SSW_NodeID_t*) self->contentFilteredTopic.associatedTopic->Data_getSecondaryKey;
    DeviceLocation_t devLoc;
    BuiltInLocationUpdateService_getDeviceLocation(*device, &devLoc);

    return LocationFilteredTopic_evalExpression(self, &devLoc);
}

rc_t
LocationFilteredTopic_evalExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc) {
    assert(self);
    assert(devLoc);

    while (self->filterstate.currentPosition < self->expressionLength) {
        rc_t ret;
        ret = s_readFunction(self);
        if (ret != SDDS_RT_OK) {
            Log_error("s_readFunction failed.\n");
            return SDDS_RT_FAIL;
        }

        ret = s_readGeometry(self);
        if (ret != SDDS_RT_OK) {
            Log_error("s_readGeometry failed.\n");
            return SDDS_RT_FAIL;
        }

        ret = s_processExpression(self, devLoc);
        if (ret != SDDS_RT_OK) {
            Log_error("s_processExpression failed.\n");
            return SDDS_RT_FAIL;
        }
    }

    if (self->filterstate.result) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

static char*
s_readWord(char* filterExpr, char* word) {
    assert(filterExpr);
    assert(word);

    int i = 0;
    while((filterExpr[i] != ' ')
          && (filterExpr[i] != '\n')
          && (filterExpr[i] != '\0')
          && (i < EXPR_WORD_MAX_LEN)) {
        word[i] = filterExpr[i];
        i++;
    }
    word[i] = '\0';
    if ((filterExpr[i] == '\0')
        || (i == EXPR_WORD_MAX_LEN)) {
        return NULL;
    }
    return &filterExpr[i+1];
}

static rc_t
s_encodeWord(LocationFilteredTopic_t* self, char* word) {
    assert(self);
    assert(word);

    if (self->expressionLength >= SDDS_FILTER_EXPR_MAX_LEN) {
        return SDDS_RT_FAIL;
    }

    if (strcmp(word, EXPR_FUNC_EQUALS_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_EQUALS_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_DISJOINT_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_DISJOINT_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_INTERSECTS_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_INTERSECTS_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_TOUCHES_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_TOUCHES_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_CROSSES_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_CROSSES_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_WITHIN_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_WITHIN_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_CONTAINS_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_CONTAINS_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_FUNC_OVERLAPS_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_FUNC_OVERLAPS_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_CONN_AND_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_CONN_AND_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_CONN_OR_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_CONN_OR_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else if (strcmp(word, EXPR_NOT_STR) == 0) {
        self->filterExpression[self->expressionLength] = EXPR_NOT_VAL;
        self->expressionLength++;
        return SDDS_RT_OK;
    }
    else {
        self->filterExpression[self->expressionLength] = atoi(word);
        self->expressionLength++;
        return SDDS_RT_OK;
    }
}

static rc_t
s_readFunction(LocationFilteredTopic_t* self) {
    assert(self);

    if ((self->filterExpression[self->filterstate.currentPosition] >= EXPR_FUNC_EQUALS_VAL)
            && (self->filterExpression[self->filterstate.currentPosition] <= EXPR_FUNC_OVERLAPS_VAL)) {
        self->filterstate.expression.negation = false;
        self->filterstate.expression.functionID = self->filterExpression[self->filterstate.currentPosition];
        self->filterstate.currentPosition++;
        return SDDS_RT_OK;
    }
    else if (self->filterExpression[self->filterstate.currentPosition] == EXPR_NOT_VAL) {
        self->filterstate.expression.negation = true;
        self->filterstate.currentPosition++;
        return s_readNotFunction(self);
    }

    return SDDS_RT_FAIL;
}

static rc_t
s_readNotFunction(LocationFilteredTopic_t* self) {
    assert(self);

    if ((self->filterExpression[self->filterstate.currentPosition] >= EXPR_FUNC_EQUALS_VAL)
            && (self->filterExpression[self->filterstate.currentPosition] <= EXPR_FUNC_OVERLAPS_VAL)) {
        self->filterstate.expression.functionID = self->filterExpression[self->filterstate.currentPosition];
        self->filterstate.currentPosition++;
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}

static rc_t
s_readGeometry(LocationFilteredTopic_t* self) {
    assert(self);

    self->filterstate.expression.geometryID = self->filterExpression[self->filterstate.currentPosition];
    self->filterstate.currentPosition++;
    return SDDS_RT_OK;
}

static rc_t
s_processExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc) {
    assert(self);
    assert(self->filterstate.expression.functionID);
    assert(self->filterstate.expression.geometryID);
    assert(devLoc);

    Geometry_t* geo = GeometryStore_getGeometry(&self->geometryStore, self->filterstate.expression.geometryID);
    if (geo == NULL) {
        Log_error("Geometry %d not found.\n", self->filterstate.expression.geometryID);
        return SDDS_RT_FAIL;
    }

    bool_t result;
    switch (self->filterstate.expression.functionID) {
    case EXPR_FUNC_EQUALS_VAL:
        result = Geometry_eaquals((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_DISJOINT_VAL:
        result = Geometry_disjoint((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_INTERSECTS_VAL:
        result = Geometry_intersects((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_TOUCHES_VAL:
        result = Geometry_touches((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_CROSSES_VAL:
        result = Geometry_crosses((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_WITHIN_VAL:
        result = Geometry_within((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_CONTAINS_VAL:
        result = Geometry_contains((Geometry_t*) &devLoc->area, geo);
        break;
    case EXPR_FUNC_OVERLAPS_VAL:
        result = Geometry_overlaps((Geometry_t*) &devLoc->area, geo);
        break;
    default:
        Log_error("Unknown function.");
        return SDDS_RT_FAIL;
    }

    if (self->filterstate.expression.negation) {
        result = !result;
    }

    switch (self->filterstate.connector) {
    case CONNECTOR_NONE:
        self->filterstate.result = result;
        break;
    case CONNECTOR_AND:
        self->filterstate.result = self->filterstate.result && result;
        break;
    case CONNECTOR_OR:
        self->filterstate.result = self->filterstate.result || result;
        break;
    default:
        Log_error("Unknown connector.");
        return SDDS_RT_FAIL;
    }

    self->filterstate.connector = CONNECTOR_NONE;
    self->filterstate.expression.negation = false;
    self->filterstate.expression.functionID = 0;
    self->filterstate.expression.geometryID = 0;

    if (self->filterstate.currentPosition < self->expressionLength) {
        return s_readConnector(self);
    }
    return SDDS_RT_OK;
}

static rc_t
s_readConnector(LocationFilteredTopic_t* self) {
    assert(self);

    if (self->filterExpression[self->filterstate.currentPosition] == EXPR_CONN_AND_VAL) {
        self->filterstate.connector = CONNECTOR_AND;
        self->filterstate.currentPosition++;
        return SDDS_RT_OK;
    }
    else if (self->filterExpression[self->filterstate.currentPosition] == EXPR_CONN_OR_VAL) {
        self->filterstate.connector = CONNECTOR_OR;
        self->filterstate.currentPosition++;
        return SDDS_RT_OK;
    }
    else {
        Log_error("Unknown connector.");
        return SDDS_RT_FAIL;
    }
}
