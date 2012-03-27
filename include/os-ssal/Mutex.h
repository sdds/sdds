/*
 * Mutex.h
 *
 *  Created on: 21 Feb 2011
 *      Author: k_beckma
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "os-ssal/SSW.h"
struct Mutex_t;
typedef struct Mutex_t* Mutex;

Mutex Mutex_create(void);

ssw_rc_t Mutex_init(Mutex mutex);

ssw_rc_t Mutex_up(Mutex mutex);
ssw_rc_t Mutex_down(Mutex mutex);

#endif /* MUTEX_H_ */
