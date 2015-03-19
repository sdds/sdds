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

#include "Memory.h"
#include "Network.h"
#include "../Foo.h"
#include "OSAL.h"

rc_t Memory_init(void)
{
    return SDDS_RT_OK;
}    


rc_t Memory_allocNetFrame(NetFrameBuff* buff)
{
    size_t size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
    size += sizeof(struct NetFrameBuff_t);

//    *buff = malloc(size);
    *buff = osal_mem_alloc(size);
    if (*buff == NULL)
	return SDDS_RT_NOMEM;
    else
	return SDDS_RT_OK;
}

rc_t Memory_allocLocator(struct Locator_t** loc){
  //  *loc = calloc(1, Network_locSize());
    *loc = osal_mem_alloc(Network_locSize());
    if (*loc == NULL)
	return SDDS_RT_NOMEM;
    else
	return SDDS_RT_OK;
}
