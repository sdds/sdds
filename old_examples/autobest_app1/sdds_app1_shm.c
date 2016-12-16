/*
################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#include <hv.h>
#include <assert.h>
#include <hv_error.h>
#include <app.id.h>
#include "sdds_app1_shm.h"
shm_sdds_app1_t* g_shm_sdds_app1;

int shm_init(void){
    addr_t base;
    size_t size;
    unsigned int err;
    err = sys_shm_iterate(CFG_SHM_sdds_shm_sdds_app1, &base, &size);
    assert (err == E_OK);
    assert (size >= sizeof(shm_sdds_app1_t));
    g_shm_sdds_app1 = (shm_sdds_app1_t*) base;
    return E_OK;
}
/*
################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
