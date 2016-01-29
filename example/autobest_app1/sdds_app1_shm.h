
/*  =========================================================================
    sdds - "Autobest sdds"


################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
    =========================================================================
*/

#ifndef __SDDS_AUTOBEST_SDDS_APP1_SHM_H_
#define __SDDS_AUTOBEST_SDDS_APP1_SHM_H_

#include <stdio.h>
#include "ipc-ds.h"
#include "beta-ds.h"
int shm_init(void);

struct shm_seg_sdds_app1_ipc{
    uint8_t type;
    uint8_t dds_obj;
    Ipc data;
} __aligned(8);
typedef struct shm_seg_sdds_app1_ipc shm_seg_sdds_app1_ipc_t;

struct shm_seg_sdds_app1_beta{
    uint8_t type;
    uint8_t dds_obj;
    Beta data;
} __aligned(8);
typedef struct shm_seg_sdds_app1_beta shm_seg_sdds_app1_beta_t;

struct shm_sdds_app1{
    shm_seg_sdds_app1_ipc_t ipc_seg;
    shm_seg_sdds_app1_beta_t beta_seg;
} __aligned(8);

typedef struct shm_sdds_app1 shm_sdds_app1_t;

extern shm_sdds_app1_t* g_shm_sdds_app1;

/*
################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#endif
