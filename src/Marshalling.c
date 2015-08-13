/*
 * =====================================================================================
 *
 *       Filename:  Marshalling.c
 *
 *    Description:  impl file for the marshalling class
 *
 *        Version:  1.0
 *        Created:  25.02.2010 11:44:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "Marshalling.h"
#include "SNPS.h"
#include <stdio.h>

void encode(byte_t* buff, byte_t* d, size_t size);
void decode(byte_t* buff, byte_t* d, size_t size);

rc_t Marshalling_enc_SubMsg(byte_t* buff, uint8_t type, uint8_t value)
{
    uint8_t write = (type | (value << 4));
    encode(buff, &write, sizeof(uint8_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_SubMsg(byte_t* buff, uint8_t type, uint8_t* value)
{
    uint8_t read;

    decode(buff, &read, sizeof(uint8_t));
    if (( read & 0x0f) != type){
	return SDDS_RT_FAIL;
    }
    *value = (read >> 4) & 0x0f;

    return SDDS_RT_OK;
}
rc_t Marshalling_enc_bool(byte_t* buff, bool_t* d)
{
    encode(buff, (byte_t*)d, sizeof(bool_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_enc_string(byte_t* buff, uint8_t* d, int size)
{
    int i;
    rc_t ret;

    for(i = 0; i < size; i++) {
        ret = Marshalling_enc_uint8(buff + i, d + i);
        if(ret != SDDS_RT_OK)
            return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}
rc_t Marshalling_enc_int8(byte_t* buff, int8_t* d)
{
    encode(buff, (byte_t*)d, sizeof(int8_t));
    return SDDS_RT_OK;

}
rc_t Marshalling_enc_uint8(byte_t* buff, uint8_t* d)
{
    encode(buff, (byte_t*)d, sizeof(uint8_t));
    return SDDS_RT_OK;

}
rc_t Marshalling_enc_int16(byte_t* buff, int16_t* d)
{
    encode(buff, (byte_t*)d, sizeof(int16_t));
    return SDDS_RT_OK;

}
rc_t Marshalling_enc_uint16(byte_t* buff, uint16_t* d)
{
    encode(buff, (byte_t*)d, sizeof(uint16_t));
    return SDDS_RT_OK;

}
rc_t Marshalling_enc_int32(byte_t* buff, int32_t* d)
{
    encode(buff, (byte_t*)d, sizeof(int32_t));
    return SDDS_RT_OK;

}
rc_t Marshalling_enc_uint32(byte_t* buff, uint32_t* d)
{

    encode(buff, (byte_t*)d, sizeof(uint32_t));
    return SDDS_RT_OK;
}
/*
rc_t Marshalling_enc_int64(byte_t* buff, int64_t* d)
{

    encode(buff, (byte_t*)d, sizeof(int64_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_enc_uint64(byte_t* buff, uint64_t* d)
{

    encode(buff, (byte_t*)d, sizeof(uint64_t));
    return SDDS_RT_OK;
}
*/

rc_t Marshalling_dec_bool(byte_t* buff, bool_t* d)
{
    decode(buff, (byte_t*) d, sizeof(bool_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_string(byte_t* buff, uint8_t* d, int size) {
    int i;
    rc_t ret;

    for(i = 0; i < size; i++) {
        ret = Marshalling_dec_uint8(buff + i, d + i);
        if(ret == SDDS_RT_FAIL)
            return SDDS_RT_FAIL;

    }
    d[i] = '\0';

    return SDDS_RT_OK;
}
rc_t Marshalling_dec_int8(byte_t* buff, int8_t* d)
{
    decode(buff, (byte_t*) d, sizeof(int8_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_uint8(byte_t* buff, uint8_t* d)
{
    decode(buff, (byte_t*) d, sizeof(uint8_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_int16(byte_t* buff, int16_t* d)
{
    decode(buff, (byte_t*) d, sizeof(int16_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_uint16(byte_t* buff, uint16_t* d)
{
    decode(buff, (byte_t*) d, sizeof(uint16_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_int32(byte_t* buff, int32_t* d)
{
    decode(buff, (byte_t*) d, sizeof(int32_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_uint32(byte_t* buff, uint32_t* d)
{
    decode(buff, (byte_t*) d, sizeof(uint32_t));
    return SDDS_RT_OK;
}
/*
rc_t Marshalling_dec_int64(byte_t* buff, int64_t* d)
{
    decode(buff, (byte_t*) d, sizeof(int64_t));
    return SDDS_RT_OK;
}
rc_t Marshalling_dec_uint64(byte_t* buff, uint64_t* d)
{
    decode(buff, (byte_t*) d, sizeof(uint64_t));
    return SDDS_RT_OK;
}
*/

void decode(byte_t* buff, byte_t* d, size_t size)
{

#ifdef SDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN
    d += size;
    for (size_t i = 0; i < size; i++){
	*(--d) = *(buff++);
    }
#else

    for (size_t i = 0; i < size; i++){
	*(d++) = *(buff++);
    }
#endif
}
void encode(byte_t* buff, byte_t* datum, size_t size)
{
#ifdef SDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN

    datum += size;
    for (size_t i = 0; i < size; i++){
	*(buff++) = *(--datum);
    }
#else

    for (size_t i = 0; i < size; i++){
	*(buff++) = *(datum++);
    }
#endif
}
