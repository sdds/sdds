/*
 * FilteredDataReader.h
 *
 *  Created on: Aug 20, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_FILTEREDDATAREADER_H_
#define SDDS_INCLUDE_SDDS_FILTEREDDATAREADER_H_

#include "sDDS.h"
#include "LocationFilteredTopic.h"

struct FilteredDataReader {
    DataReader_t dataReader;
    LocationFilteredTopic_t* locationFilteredTopic;
};
typedef struct FilteredDataReader FilteredDataReader_t;

void
FilteredDataReader_init(FilteredDataReader_t* self, uint8_t id, LocationFilteredTopic_t* topic, On_Data_Avail_Listener listener);

#endif /* SDDS_INCLUDE_SDDS_FILTEREDDATAREADER_H_ */
