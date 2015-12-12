/*
 * =====================================================================================
 *
 *       Filename:  TopicMarshalling.h
 *
 *    Description:  interface for the marshalling of the data assigend to a
 *    topic
 *
 *        Version:  1.0
 *        Created:  24.02.2010 19:09:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  TOPICMARSHALLING_H_INC
#define  TOPICMARSHALLING_H_INC

#ifdef SDDS_PLATFORM_autobest
#ifndef stderr
#define stderr 2
#endif
#define fprintf(a, b, args ...) printf(b, ## args)
#endif

#include "sdds_types.h"

typedef rc_t (TopicMarshalling_encode_fn) (NetBuffRef_t* buff, Data data, size_t* size);
typedef rc_t (TopicMarshalling_decode_fn) (NetBuffRef_t* buff, Data data, size_t* size);

#endif   /* ----- #ifndef TOPICMARSHALLING_H_INC  ----- */
