/*
 * LocationFilteredTopic.c
 *
 *  Created on: Aug 10, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "LocationFilteredTopic.h"
#include "BuiltInLocationUpdateService.h"
#include "os-ssal/LocationService.h"
#include "string.h"

#ifdef TEST_EVAL_LOCATION_FILTER_RIOT
#include "xtimer.h"
#endif

#if defined(TEST_EVAL_LOCATION_FILTER_LINUX) || defined(TEST_EVAL_LOCATION_SET_FILTER_LINUX)
#include <sys/time.h>
#endif

#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED

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
LocationFilteredTopic_create(LocationFilteredTopic_t* self, Topic_t* topic, char* filterExpression, GeometryStore_t* store) {
    assert(self);
    assert(store);

    self->contentFilteredTopic.associatedTopic = topic;
    self->geometryStore = store;

    self->filterstate.connector = CONNECTOR_NONE;
    self->filterstate.expression.negation = false;
    self->filterstate.expression.functionID = 0;
    self->filterstate.expression.geometryID = 0;

    if (filterExpression == NULL) {
        return SDDS_RT_OK;
    }

    return LocationFilteredTopic_setFilter(self, filterExpression);
}

rc_t
LocationFilteredTopic_setFilter(LocationFilteredTopic_t* self, char* filterExpression) {
    assert(self);
    assert(filterExpression);

#ifdef TEST_EVAL_LOCATION_SET_FILTER_LINUX
    struct timeval start;
    gettimeofday(&start, NULL);
#endif

    memset(self->filterExpression, 0, SDDS_FILTER_EXPR_MAX_LEN);
    char word[EXPR_WORD_MAX_LEN];
    char* pos = filterExpression;
    do {
        pos = s_readWord(pos, word);
        rc_t ret = s_encodeWord(self, word);
        if (ret != SDDS_RT_OK) {
            Log_error("Filter expression too long.\n");
            return SDDS_RT_FAIL;
        }
    } while (pos != NULL);

#ifdef TEST_EVAL_LOCATION_SET_FILTER_LINUX
    struct timeval end;
    gettimeofday(&end, NULL);
    time_t start_usec = (start.tv_sec * 1000000 + start.tv_usec);
    time_t end_usec = (end.tv_sec * 1000000 + end.tv_usec);
    time_t duration = (end_usec - start_usec);

    printf("setFilterEval s,e,d (us): %lu.%lu, %lu.%lu, %lu\n", start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec, duration); 
    fflush(stdout);
    exit(0);
#endif
    return SDDS_RT_OK;
}

rc_t
LocationFilteredTopic_evalSample(LocationFilteredTopic_t* self, Data data) {
    SSW_NodeID_t* device = (SSW_NodeID_t*) self->contentFilteredTopic.associatedTopic->Data_getSecondaryKey(data);
    DeviceLocation_t devLoc;
    rc_t ret = BuiltInLocationUpdateService_getDeviceLocation(*device, &devLoc);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_OK;
    }

    return LocationFilteredTopic_evalExpression(self, &devLoc);
}

rc_t
LocationFilteredTopic_evalExpression(LocationFilteredTopic_t* self, DeviceLocation_t* devLoc) {
    assert(self);
    assert(devLoc);

#ifdef TEST_EVAL_LOCATION_FILTER_RIOT
    timex_t start;
    xtimer_now_timex(&start);
#endif

#ifdef TEST_EVAL_LOCATION_FILTER_LINUX
    struct timeval start;
    gettimeofday(&start, NULL);
#endif

    while (self->filterstate.currentPosition < self->expressionLength) {
        rc_t ret;
        ret = s_readFunction(self);
        if (ret != SDDS_RT_OK) {
            Log_error("s_readFunction failed.\n");
            self->filterstate.currentPosition = 0;
            return SDDS_RT_FAIL;
        }

        ret = s_readGeometry(self);
        if (ret != SDDS_RT_OK) {
            Log_error("s_readGeometry failed.\n");
            self->filterstate.currentPosition = 0;
            return SDDS_RT_FAIL;
        }

        ret = s_processExpression(self, devLoc);
        if (ret != SDDS_RT_OK) {
            Log_error("s_processExpression failed.\n");
            self->filterstate.currentPosition = 0;
            return SDDS_RT_FAIL;
        }
    }

#ifdef TEST_EVAL_LOCATION_FILTER_RIOT
    timex_t end;
    xtimer_now_timex(&end);

    long start_usec = (start.seconds * 1000000 + start.microseconds);
    long end_usec = (end.seconds * 1000000 + end.microseconds);
    long duration = (end_usec - start_usec);

    printf("filterEval s,e,d (us): %ld.%ld, %ld.%ld, %ld\n", start.seconds, start.microseconds, end.seconds, end.microseconds, duration); 

#endif

#ifdef TEST_EVAL_LOCATION_FILTER_LINUX
    struct timeval end;
    gettimeofday(&end, NULL);
    time_t start_usec = (start.tv_sec * 1000000 + start.tv_usec);
    time_t end_usec = (end.tv_sec * 1000000 + end.tv_usec);
    time_t duration = (end_usec - start_usec);

    printf("filterEval s,e,d (us): %lu.%lu, %lu.%lu, %lu\n", start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec, duration); 
    fflush(stdout);
#endif


    if (self->filterstate.result) {
        self->filterstate.currentPosition = 0;
        return SDDS_RT_OK;
    }

    self->filterstate.currentPosition = 0;
    return SDDS_RT_FAIL;
}

rc_t
LocationFilteredTopic_equals(LocationFilteredTopic_t* t1, LocationFilteredTopic_t* t2) {
    assert(t1);
    assert(t2);
    bool_t sameTopic = (t1->contentFilteredTopic.associatedTopic == t2->contentFilteredTopic.associatedTopic);
    if (!sameTopic) {
        return SDDS_RT_FAIL;
    }

    bool_t sameFilter = t1->expressionLength == t2->expressionLength;
    if (!sameFilter) {
        return SDDS_RT_FAIL;
    }

    uint8_t i = 0;
    uint8_t length = t1->expressionLength;
    while (i < length) {
        if (t1->filterExpression[i] != t2->filterExpression[i]) {
            return SDDS_RT_FAIL;
        }
        i++;
    }

    return SDDS_RT_OK;
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
    assert(self->filterstate.currentPosition < self->expressionLength);

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

    Geometry_t* geo = GeometryStore_getGeometry(self->geometryStore, self->filterstate.expression.geometryID);
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
        Log_error("Unknown function %d.\n", self->filterstate.expression.functionID);
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
        Log_error("Unknown connector %d\n", self->filterstate.connector);
        self->filterstate.connector = CONNECTOR_NONE;
        self->filterstate.expression.negation = false;
        self->filterstate.expression.functionID = 0;
        self->filterstate.expression.geometryID = 0;

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
        Log_error("Unknown connector %d\n", self->filterstate.connector);
        return SDDS_RT_FAIL;
    }
}

#endif /* FEATURE_SDDS_LOCATION_FILTER_ENABLED */
