/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  Thread interface for the system softare abstraction layer
 *
 *        Version:  1.0
 *        Created:  03.03.2010 10:17:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  THREAD_H_INC
#define  THREAD_H_INC
#include "SSW.h"
#include "sdds_types.h"

struct Thread_t;
typedef struct Thread_t* Thread;

Thread Thread_create(void);

// interface for the to run method attachable to a thread
// Second parameter is the assorizated object, that the uses the thread
ssw_rc_t Thread_function(Thread _this, void* object);

ssw_rc_t Thread_init(Thread _this, void*(*function)(void* object), void* obj);

ssw_rc_t Thread_run(Thread _this);
ssw_rc_t Thread_stop(Thread _this);
ssw_rc_t Thread_usleep(Thread _this, uint16_t usecSleep);
ssw_rc_t Thread_sleep(Thread _this, uint16_t secSleep);

#endif   /* ----- #ifndef THREAD_H_INC  ----- */
