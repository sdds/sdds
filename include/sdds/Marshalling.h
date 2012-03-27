/*
 * =====================================================================================
 *
 *       Filename:  Marshalling.h
 *
 *    Description:  Marshalling class, holding the en- and decode functions for 
 *    the primitive datatypes
 *
 *        Version:  1.0
 *        Created:  24.02.2010 19:50:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  MARSHALLING_H_INC
#define  MARSHALLING_H_INC

#include "sdds_types.h"

rc_t Marshalling_enc_bool(byte_t* buff, bool_t* d);
rc_t Marshalling_enc_int8(byte_t* buff, int8_t* d);
rc_t Marshalling_enc_uint8(byte_t* buff, uint8_t* d);
rc_t Marshalling_enc_int16(byte_t* buff, int16_t* d);
rc_t Marshalling_enc_uint16(byte_t* buff, uint16_t* d);
rc_t Marshalling_enc_int32(byte_t* buff, int32_t* d);
rc_t Marshalling_enc_uint32(byte_t* buff, uint32_t* d);
//rc_t Marshalling_enc_int64(byte_t* buff, int64_t* d);
//rc_t Marshalling_enc_uint64(byte_t* buff, uint64_t* d);

rc_t Marshalling_dec_bool(byte_t* buff, bool_t* d);
rc_t Marshalling_dec_int8(byte_t* buff, int8_t* d);
rc_t Marshalling_dec_uint8(byte_t* buff, uint8_t* d);
rc_t Marshalling_dec_int16(byte_t* buff, int16_t* d);
rc_t Marshalling_dec_uint16(byte_t* buff, uint16_t* d);
rc_t Marshalling_dec_int32(byte_t* buff, int32_t* d);
rc_t Marshalling_dec_uint32(byte_t* buff, uint32_t* d);
//rc_t Marshalling_dec_int64(byte_t* buff, int64_t* d);
//rc_t Marshalling_dec_uint64(byte_t* buff, uint64_t* d);




rc_t Marshalling_enc_SubMsg(byte_t* buff, uint8_t type, uint8_t value);
rc_t Marshalling_dec_SubMsg(byte_t* buff, uint8_t type, uint8_t* value);
#endif   /* ----- #ifndef MARSHALLING_H_INC  ----- */
