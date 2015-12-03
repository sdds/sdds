/*
 * =====================================================================================
 *
 *       Filename:  Memory.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  22.02.2010 16:03:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "os-ssal/Memory.h"

ssw_rc_t
Memory_init(void) {
    return SDDS_RT_OK;
}

void*
Memory_alloc(size_t size) {
    return malloc(size);
}

void
Memory_free(void* ptr) {
    free(ptr);
}