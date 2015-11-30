/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#ifndef __DATA_READER_H_INCLUDED__
#define __DATA_READER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Callbacks of this class

typedef void (*On_Data_Avail_Listener)(DataReader_t *);

//  Structure of this class

struct _DataReader_t {
	Topic_t *topic;
	unsigned int id :4;
	On_Data_Avail_Listener on_data_avail_listener;
   History_t history;
};

//  Tries to take a sample from the data readers history. The provided
//  structure must match the de-serialized data for this topic. Return
//  SDDS_RT_OK if data available, otherwise SDDS_RT_NODATA.
rc_t
   DataReader_take_next_sample (DataReader_t *self, Data* data, DataInfo info);

On_Data_Avail_Listener
   DataReader_on_data_avail_listener (DataReader_t *self);

rc_t
   DataReader_set_on_data_avail_listener (DataReader_t *self,
                                          On_Data_Avail_Listener listener,
                                          const StatusMask sm);

unsigned int
   DataReader_id (DataReader_t *self);

Topic_t*
   DataReader_topic (DataReader_t *self);

History_t*
   DataReader_history (DataReader_t *self);

#ifdef __cplusplus
}
#endif

#endif
