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

#ifndef SDDS_MAX_SUBSCRIPTIONS
#define SDDS_MAX_SUBSCRIPTIONS 20
#endif

static TopicSubscription_t topicSubscription_pool[SDDS_MAX_SUBSCRIPTIONS];

static rc_t
s_Topic_getFreeTopicSubscription(uint8_t* index) {
    for (int i = 0; i < SDDS_MAX_SUBSCRIPTIONS; i++) {
        if (topicSubscription_pool[i].state == CANCELLED) {
            *index = i;
            return SDDS_RT_OK;
        }
    }
    return SDDS_RT_FAIL;
}

static rc_t
s_TopicSubcription_contains(List_t* list, BuiltinTopicKey_t participant) {
    if (list == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }

    TopicSubscription_t* data = (TopicSubscription_t*) list->first_fn(list);

    while (data != NULL) {
        if (data->participant == participant){
            return SDDS_RT_OK;
        }
        data = list->next_fn(list);
    }

    return SDDS_RT_FAIL;
}

rc_t
Topic_init() {
    for (int i = 0; i < SDDS_MAX_SUBSCRIPTIONS; i++) {
        topicSubscription_pool[i].participant = 0;
        topicSubscription_pool[i].addr = NULL;
        topicSubscription_pool[i].state = CANCELLED;
    }
    return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
rc_t
Topic_addRemoteDataSink(Topic_t* _this, Locator_t* addr, BuiltinTopicKey_t participant, SubscriptionState_t state) {

    if (_this == NULL || addr == NULL) {
        Log_error("SDDS_RT_BAD_PARAMETER\n");
        return SDDS_RT_BAD_PARAMETER;
    }

    uint8_t index = 0;
    rc_t ret = s_Topic_getFreeTopicSubscription(&index);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    TopicSubscription_t* tSub = &topicSubscription_pool[index];
    tSub->participant = participant;
    tSub->addr = addr;
    tSub->state = state;

    List_t* list = _this->dsinks.list;
    if (list->size_fn(list) == 0) {
        if (list->add_fn(list, tSub) == SDDS_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }
    else if (s_TopicSubcription_contains(list, tSub->participant) != SDDS_RT_OK) {
        if (list->add_fn(list, tSub) == SDDS_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}

rc_t
Topic_editRemoteDataSink(Topic_t* _this, BuiltinTopicKey_t participant, SubscriptionState_t state) {
    assert(_this);

    List_t* list = _this->dsinks.list;
    TopicSubscription_t* data = (TopicSubscription_t*) list->first_fn(list);

    while (data != NULL) {
        if (data->participant == participant){
            data->state = state;
            return SDDS_RT_OK;
        }
        data = list->next_fn(list);
    }
    return SDDS_RT_FAIL;
}

rc_t
Topic_removeRemoteDataSink(Topic_t* _this, BuiltinTopicKey_t participant) {
    assert(_this);

    List_t* list = _this->dsinks.list;;
    TopicSubscription_t* data = (TopicSubscription_t*) list->first_fn(list);

    while (data != NULL) {
        if (data->participant == participant){
            Locator_downRef(data->addr);
            return list->delete_fn(list);
        }
        data = list->next_fn(list);
    }
    return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
Topic_addRemoteDataSource(Topic_t* _this, Locator_t* addr, BuiltinTopicKey_t participant, SubscriptionState_t state) {
    if (_this == NULL || addr == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }

    List_t* list = _this->dsources.list;
    if (list->size_fn(list) == 0) {
        if (list->add_fn(list, addr) == SDDS_RT_FAIL) {
            return SDDS_RT_FAIL;
        }

        return SDDS_RT_OK;
    }
    else if (Locator_contains(list, addr) != SDDS_RT_OK) {
        if (list->add_fn(list, addr) == SDDS_RT_FAIL) {
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }

    return SDDS_RT_FAIL;
}

rc_t
Topic_editRemoteDataSource(Topic_t* _this, BuiltinTopicKey_t participant, SubscriptionState_t state) {
    assert(_this);

    List_t* list = _this->dsources.list;;
    TopicSubscription_t* data = (TopicSubscription_t*) list->first_fn(list);

    while (data != NULL) {
        if (data->participant == participant){
            data->state = state;
            return SDDS_RT_OK;
        }
        data = list->next_fn(list);
    }
    return SDDS_RT_FAIL;
}

rc_t
Topic_removeRemoteDataSource(Topic_t* _this, BuiltinTopicKey_t participant) {
    assert(_this);

    List_t* list = _this->dsources.list;;
    TopicSubscription_t* data = (TopicSubscription_t*) list->first_fn(list);

    while (data != NULL) {
        if (data->participant == participant){
            Locator_downRef(data->addr);
            return list->delete_fn(list);
        }
        data = list->next_fn(list);
    }
    return SDDS_RT_OK;
}
#endif
