/*
 * SDDS_Application.h
 *
 *  Created on: 22.03.2013
 *      Author: kai
 */

#ifndef SDDS_APPLICATION_H_
#define SDDS_APPLICATION_H_

#include "sdds/sdds_types.h"

rc_t SDDS_Application_init();

rc_t SDDS_Application_doWork();

rc_t SDDS_Application_start();

rc_t SDDS_Application_stop();

#endif /* SDDS_APPLICATION_H_ */
