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
 * @file      Marshalling.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Marshalling class, holding the en- and decode functions for
 * the primitive datatypes
 */



#ifndef  MARSHALLING_H_INC
#define  MARSHALLING_H_INC

#include "sdds_types.h"

rc_t
Marshalling_enc_bool(byte_t* buff, bool_t* d);
rc_t
Marshalling_enc_int8(byte_t* buff, int8_t* d);
rc_t
Marshalling_enc_uint8(byte_t* buff, uint8_t* d);
rc_t
Marshalling_enc_int16(byte_t* buff, int16_t* d);
rc_t
Marshalling_enc_uint16(byte_t* buff, uint16_t* d);
rc_t
Marshalling_enc_int32(byte_t* buff, int32_t* d);
rc_t
Marshalling_enc_uint32(byte_t* buff, uint32_t* d);
rc_t
Marshalling_enc_string(byte_t* buff, uint8_t* d, int size);
rc_t
Marshalling_enc_int64(byte_t* buff, int64_t* d);
rc_t
Marshalling_enc_uint64(byte_t* buff, uint64_t* d);

rc_t
Marshalling_dec_bool(byte_t* buff, bool_t* d);
rc_t
Marshalling_dec_int8(byte_t* buff, int8_t* d);
rc_t
Marshalling_dec_uint8(byte_t* buff, uint8_t* d);
rc_t
Marshalling_dec_int16(byte_t* buff, int16_t* d);
rc_t
Marshalling_dec_uint16(byte_t* buff, uint16_t* d);
rc_t
Marshalling_dec_int32(byte_t* buff, int32_t* d);
rc_t
Marshalling_dec_uint32(byte_t* buff, uint32_t* d);
rc_t
Marshalling_dec_string(byte_t* buff, uint8_t* d, int size);
rc_t
Marshalling_dec_int64(byte_t* buff, int64_t* d);
rc_t
Marshalling_dec_uint64(byte_t* buff, uint64_t* d);




rc_t
Marshalling_enc_SubMsg(byte_t* buff, uint8_t type, uint8_t value);
rc_t
Marshalling_enc_ExtSubMsg(byte_t* buff, uint8_t type, byte_t* value, size_t size);
rc_t
Marshalling_dec_ExtSubMsg(byte_t* buff, uint8_t type, byte_t* value, size_t size);
rc_t
Marshalling_dec_SubMsg(byte_t* buff, uint8_t type, uint8_t* value);
#endif   /* ----- #ifndef MARSHALLING_H_INC  ----- */
