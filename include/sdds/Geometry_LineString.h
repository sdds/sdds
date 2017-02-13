/*
 * Geometry_LineString.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_LINESTRING_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_LINESTRING_H_

#include "Geometry.h"

bool_t
LineString_equals(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_disjoint(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_intersects(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_touches(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_crosses(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_within(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_contains(LineString_t* self, Geometry_t* otherObject);

bool_t
LineString_overlaps(LineString_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_LINESTRING_H_ */
