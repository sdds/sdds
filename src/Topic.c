/*
 * =====================================================================================
 *
 *       Filename:  Topic.c
 *
 *    Description:  impl file for the abstract part of the topic class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 09:27:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "sDDS.h"
/*
#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
*/
rc_t
Topic_addRemoteDataSink(Topic_t* _this, Locator_t* addr) {


    if (_this == NULL || addr == NULL) {
        Log_error("SDDS_RT_BAD_PARAMETER\n");
        return SDDS_RT_BAD_PARAMETER;
    }

    List_t* list = _this->dsinks.list;
    if (list->size_fn(list) == 0) {
        if (list->add_fn(list, addr) == SDDS_SSW_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }
    else if (Locator_contains(list, addr) != SDDS_RT_OK) {
        if (list->add_fn(list, addr) == SDDS_SSW_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}
//#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
Topic_addRemoteDataSource(Topic_t* _this, Locator_t* addr) {
    if (_this == NULL || addr == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }

    List_t* list = _this->dsources.list;
    if (list->size_fn(list) == 0) {
        if (list->add_fn(list, addr) == SDDS_SSW_RT_FAIL) {
            return SDDS_RT_FAIL;
        }

        return SDDS_RT_OK;
    }
    else if (Locator_contains(list, addr) != SDDS_RT_OK) {
        if (list->add_fn(list, addr) == SDDS_SSW_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}
#endif
