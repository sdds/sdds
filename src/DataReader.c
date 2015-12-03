/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#include "sDDS.h"

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
//  ---------------------------------------------------------------------------
//  Tries to take a sample from the data readers history. The provided
//  structure must match the de-serialized data for this topic. Return
//  SDDS_RT_OK if data available, otherwise SDDS_RT_NODATA.

rc_t
DataReader_take_next_sample (DataReader_t *self, Data* data, DataInfo info)
{
    assert (self);
    assert (data);
	(void) info;
	//  Check if buffer is provided
    if (*data) {
        Sample_t* sample = sdds_History_dequeue (DataReader_history (self));
        if (sample == NULL)
            return SDDS_RT_NODATA;
		(self->topic->Data_cpy)(*data, (Data) sample->data);
        //  TODO Sample infos
        return SDDS_RT_OK;
	}
    else {
		//  TODO Implement loan
		Log_error ("No buffer for datasample is provided. Data is lost\n");
		return SDDS_RT_FAIL;
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

History_t *
DataReader_history (DataReader_t *self)
{
    assert (self);
    return &self->history;
}

#endif
