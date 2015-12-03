/*
 * =====================================================================================
 *
 *       Filename:  Memory.h
 *
 *    Description:  Class for the memory management
 *
 *        Version:  1.0
 *        Created:  22.02.2010 16:00:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  MEMORY_H_INC
#define  MEMORY_H_INC

#include "sdds/sdds_types.h"
#include "SSW.h"

ssw_rc_t
Memory_init(void);

void*
Memory_alloc(size_t size);

void
Memory_free(void* ptr);

#endif   /* ----- #ifndef MEMORY_H_INC  ----- */