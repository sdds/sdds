/*
 * Management.h
 *
 *  Created on: Jul 19, 2010
 *      Author: kai
 */

#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#if 0

struct PubMng_t {

    // IF BUILTINTOPIC AND PUB AND LESSOFTEN
    uint8_t builtinPubCount;
    // ENDIF

};

struct SubMng_t {
    uint8_t placeholder;
};

struct Management_t {
    struct PubMng_t pub;
    struct SubMng_t sub;
    Task mngTask;

};
typedef struct Management_t* Management;

extern Management management;

/**
 * Init function for the gloal management system of sdds
 * Have to be called at the initialization of the system
 */
rc_t
Management_init(void);

#endif

#endif /* MANAGEMENT_H_ */
