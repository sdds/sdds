/*
 * Application.h
 *
 *  Created on: Aug 9, 2010
 *      Author: kai
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "sDDS/NetBuffRef.h"

/**
 * Application Interface for the usage of sdds with BitCloud,
 * shall be implemented by the application developer
 */

/**
 * Prototype for the application run methode.
 * Invocation sould not last longer than 10 ms
 * (see BitCloud User Guide)
 */

int sdds_app_run(void);

/**
 * Initializes the application
 */
int sdds_app_init(void);

/*
 * Function for reinitialisation. for example after a power shortage
 */
int sdds_app_reInit(void);

/**
 * function for the application code regarding an poweroff event
 */

int sdds_app_poweroff(void);

int sdds_app_tmpMsg(NetBuffRef buf);

void sdds_app_sendOK(void);
void sdds_app_fail(void);


#endif /* SDDS_ZIGBEE_H_ */
