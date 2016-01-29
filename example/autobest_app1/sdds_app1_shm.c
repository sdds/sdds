
#include <hv.h>
#include <assert.h>
#include <hv_error.h>
#include "sdds_app1_shm.h"
shm_sdds_app1_t* g_shm_sdds_app1;

int shm_init(void){
    addr_t base;
    size_t size;
    unsigned int err;
    err = sys_shm_iterate(0, &base, &size);
    assert (err == E_OK);
    assert (size >= sizeof(shm_sdds_app1_t));
    g_shm_sdds_app1 = (shm_sdds_app1_t*) base;
    return E_OK;
}
