/*  =========================================================================
    DataReader -

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#include "sDDS.h"

struct _DataReader_t {
	Topic_t *topic;
	unsigned int id :4;
	On_Data_Avail_Listener on_data_avail_listener;
};

DataReader_t data_readers[SDDS_MAX_DATA_READERS];
DataReader_t *readers[SDDS_MAX_DATA_READERS];

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)

rc_t
DataReader_init (void) {
#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Woverflow"
#endif
    int index;
    for (index = 1; index < SDDS_MAX_DATA_READERS; index++) {
        readers[index] = NULL;
    }
#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#endif
	return SDDS_RT_OK;
}

//  ---------------------------------------------------------------------------
//  Creates a new DataReader object

DataReader_t *
DataReader_new (Topic_t *topic, Qos qos, Listener listener, StatusMask sm)
{
	qos = qos;
	sm = sm;

	DataReader_t *self = NULL;
    int index;
    for (index = 0; index < SDDS_MAX_DATA_READERS; index++) {
        if (!readers[index]) {
            self = &data_readers[index];
            readers[index] = self;
            break;
        }
    }
    if (self) {
        self->id = SDDS_MAX_DATA_READERS - index;
        self->topic = topic;
        self->on_data_avail_listener = NULL;
    }
	return self;
}

//  ---------------------------------------------------------------------------
//  Deletes a DataReader object

void
DataReader_delete (DataReader_t **self_p)
{
    if (*self_p) {
        DataReader_t *self;

        int index;
        for (index = 1; index < SDDS_MAX_DATA_READERS; index++) {
            if (readers[index] == self) {
                readers[index] = NULL;
            }
        }
        self = NULL;
    }
}

On_Data_Avail_Listener
DataReader_on_data_avail_listener (DataReader_t *self)
{
    return self->on_data_avail_listener;
}

rc_t
   DataReader_set_on_data_avail_listener (DataReader_t *self,
                                          On_Data_Avail_Listener listener,
                                          const StatusMask sm)
{
	self->on_data_avail_listener = listener;
	return SDDS_RT_OK;
}

unsigned int
DataReader_id (DataReader_t *self)
{
    return self->id;
}

Topic_t *
DataReader_topic (DataReader_t *self)
{
    return self->topic;
}
#endif
