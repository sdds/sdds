/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      DataReader.h
 * @author    TODO
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * DDS DataReader representation, allows the application to 
 * declare the data it wishes to receive.
 */

#ifndef __DATA_READER_H_INCLUDED__
#define __DATA_READER_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Callbacks of this class

typedef void (* On_Data_Avail_Listener)(DataReader_t*);
typedef rc_t (* Fn_pushData)(DataReader_t* self, NetBuffRef_t* buff);

//  Structure of this class

struct _DataReader_t {
    Topic_t* topic;
    unsigned int id : 4;
    On_Data_Avail_Listener on_data_avail_listener;
    History_t history;
    Fn_pushData fn_pushData;
};

void
DataReader_init(DataReader_t* self, uint8_t id, Topic_t* topic, On_Data_Avail_Listener listener);

//  Tries to take a sample from the data readers history. The provided
//  structure must match the de-serialized data for this topic. Return
//  SDDS_RT_OK if data available, otherwise SDDS_RT_NODATA.
rc_t
DataReader_take_next_sample(DataReader_t* self, Data* data, DataInfo info);

On_Data_Avail_Listener
DataReader_on_data_avail_listener(DataReader_t* self);

rc_t
DataReader_set_on_data_avail_listener(DataReader_t* self,
                                      On_Data_Avail_Listener listener,
                                      const StatusMask sm);

unsigned int
DataReader_id(DataReader_t* self);

Topic_t*
DataReader_topic(DataReader_t* self);

History_t*
DataReader_history(DataReader_t* self);

rc_t
DataReader_pushData(DataReader_t* self, NetBuffRef_t* buff);

rc_t
DataReader_s_pushData(DataReader_t* self, NetBuffRef_t* buff);

#ifdef __cplusplus
}
#endif

#endif
