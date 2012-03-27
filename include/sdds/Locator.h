/*
 * =====================================================================================
 *
 *       Filename:  Locator.h
 *
 *    Description:  Locator interface for the abstractaion of the network 
 *    address
 *
 *        Version:  1.0
 *        Created:  24.02.2010 12:54:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  LOCATOR_H_INC
#define  LOCATOR_H_INC
#include "sdds_types.h"

#define SDDS_LOCATOR_STATE_EMPTY	0x00
#define SDDS_LOCATOR_STATE_FILLED	0x01
#define SDDS_LOCATOR_STATE_REF		0x02

#define SDDS_LOCATOR_TYPE_UNI		0x00
#define SDDS_LOCATOR_TYPE_MULTI		0x01
#define SDDS_LOCATOR_TYPE_BROAD		0x02


struct Locator_t{
    struct Locator_t* next;
    // TODO state reicht nicht ref counter?
    // wie geht das mit den bitmaps?
    // 4 bit refconter?
    unsigned int type		: 2;
    _Bool isEmpty		: 1;
    _Bool isDest		: 1;
    _Bool isSender		: 1;
    unsigned int refCount	: 4;
};
typedef struct Locator_t* Locator;

bool_t Locator_isEqual(Locator l1, Locator l2);
#define Locator_upRef(x) if (x->refCount <= 15) x->refCount++ 
#define Locator_downRef(x) if (x->refCount > 0) x->refCount-- 

rc_t Locator_add(Locator head, Locator newL);
rc_t Locator_del(Locator head, Locator toDel);

#endif   /* ----- #ifndef LOCATOR_H_INC  ----- */
