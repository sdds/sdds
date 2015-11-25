/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#include "sDDS.h"

//  Structure of this class

struct _DataReader_t {
	Topic_t *topic;
	unsigned int id :4;
	On_Data_Avail_Listener on_data_avail_listener;
};

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)

static DataReader_t objects[SDDS_DATA_READER_MAX_OBJS];
static BitArray_t *objectAllocation;


//  ----------------------------------------------------------------------------
//  Initialize this class

rc_t
DataReader_init (void) {
    objectAllocation = BitArray_new ();
	return SDDS_RT_OK;
}

//  ---------------------------------------------------------------------------
//  Creates a new DataReader object

DataReader_t *
DataReader_new (Topic_t *topic, Qos qos, Listener listener, StatusMask sm)
{
    assert (objectAllocation);
	qos = qos;
	sm = sm;

    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        if (!BitArray_check (objectAllocation, index)) {
            BitArray_set (objectAllocation, index);
            DataReader_t *self = &objects[index];
            // Initialize object properties
            self->id = index;
            self->topic = topic;
            self->on_data_avail_listener = NULL;
            return self;
        }
    }
	return NULL;
}

//  ---------------------------------------------------------------------------
//  Deletes a DataReader object

void
DataReader_destroy (DataReader_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        DataReader_t *self = *self_p;

        uint8_t index;
        for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
            if (self == &objects[index]) {
                BitArray_clear (objectAllocation, index);
                //  Cleanup object properties
                self->id = 0;
                self->topic = NULL;
                self->on_data_avail_listener = NULL;
            }
        }
    }
}

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
	Msg_t *msg = NULL;

    //  If there a no data in the history return immediately
	rc_t ret = Topic_getNextMsg (self->topic, &msg);
	if (ret == SDDS_RT_NODATA)
		return SDDS_RT_NODATA;

	//  Check if buffer is provided
    if (*data) {
		//  Copy the data
		Data newData;
		Msg_getData(msg, &newData);

		(self->topic->Data_cpy)(*data, newData);
		//  Free the msg
		Msg_init(msg, NULL);
	}
    else {
		//  TODO Implement loan
		Log_error ("No buffer for datasample is provided. Data is lost\n");
		return SDDS_RT_FAIL;
	}
	//  TODO Sample infos
	return SDDS_RT_OK;
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
