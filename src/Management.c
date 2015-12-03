/*
 * Management.c
 *
 *  Created on: Jul 19, 2010
 *      Author: kai
 */

#include "Management.h"
#include "os-ssal/SSW.h"
#include "BuiltinTopic.h"

#if 0
static struct Management_t staticMng;

Management management = &staticMng;

void
Management_work(void* data);

rc_t
Management_init(void) {
    // IF MNG AND TASK
    management->mngTask = Task_create();
    Task_init(management->mngTask, Management_work, NULL);
    // IF REGULARY
    Task_start(management->mngTask, 0, SDDS_MNG_WORKER_CYCLE_TIME, SDDS_SSW_TaskMode_repeat);
    // ENDIF
    // ENDIF
    // IF BUILTINTOPIC AND PUB AND LESSOFTEN
    management->pub.builtinPubCount = 0;
    //ENDIF

    return SDDS_RT_OK;
}

void
Management_work(void* data) {
    // IF BUILTINTOPIC AND PUB AND ALL
    // IF LESSOFTERN
    if (management->pub.builtinPubCount++ > SDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER) {
        management->pub.builtinPubCount = 0;



        // ENDIF
        BuiltinTopic_publishAll();
        // if BUILTINTOPIC AND PUB AND ? TODO
    }

}
#endif