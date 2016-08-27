/*
 * ContentFilteredTopic.h
 *
 *  Created on: Aug 9, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_
#define SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_

#include "sDDS.h"
#include "Topic.h"

struct ContentFilteredTopic {
    Topic_t* associatedTopic;
};
typedef struct ContentFilteredTopic ContentFilteredTopic_t;



#endif /* SDDS_INCLUDE_SDDS_CONTENTFILTEREDTOPIC_H_ */
