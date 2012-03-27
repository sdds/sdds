/*
 * =====================================================================================
 *
 *       Filename:  NetFrameBuff.h
 *
 *    Description:  interface for the networkbuffer
 *
 *        Version:  1.0
 *        Created:  24.02.2010 13:59:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  NETBUFF_H_INC
#define  NETBUFF_H_INC
struct NetFrameBuff_t
{
    size_t size;
};
typedef struct NetFrameBuff_t* NetFrameBuff;

#endif   /* ----- #ifndef NETBUFF_H_INC  ----- */
