/*
 * FilteredDataReader.c
 *
 *  Created on: Aug 20, 2016
 *      Author: o_dedi
 */

#include "FilteredDataReader.h"

static rc_t
s_pushData(DataReader_t* self, NetBuffRef_t* buff);

void
FilteredDataReader_init(DataReader_t* self, uint8_t id, LocationFilteredTopic_t* topic, On_Data_Avail_Listener listener) {
    assert(self);
    assert(topic);

    self->id = id;
    self->topic = (Topic_t*) topic;
    self->on_data_avail_listener = listener;
    self->fn_pushData = s_pushData;
}

static rc_t
s_pushData(DataReader_t* self, NetBuffRef_t* buff) {
    return SDDS_RT_OK;
}
