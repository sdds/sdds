/*
 * =====================================================================================
 *
 *       Filename:  RecvStateMachine.h
 *
 *    Description:  Class for the parsing of frames received.
 *
 *        Version:  1.0
 *        Created:  05.03.2010 13:27:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  RECVSTATEMACHINE_H_INC
#define  RECVSTATEMACHINE_H_INC

#include "sdds_types.h"

rc_t
FreshFrame_readSubMsg(NetBuffRef buff);
rc_t
DomainSet_readSubMsg(NetBuffRef buff);
rc_t
TopicSet_readSubMsg(NetBuffRef buff);
rc_t
DataSet_readSubMsg(NetBuffRef buff);

#endif   /* ----- #ifndef RECVSTATEMACHINE_H_INC  ----- */