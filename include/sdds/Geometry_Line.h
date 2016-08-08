/*
 * Geometry_Line.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_LINE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_LINE_H_

#include "Geometry.h"

bool_t
Line_equals(Line_t* self, Geometry_t* otherObject);

bool_t
Line_disjoint(Line_t* self, Geometry_t* otherObject);

bool_t
Line_intersects(Line_t* self, Geometry_t* otherObject);

bool_t
Line_touches(Line_t* self, Geometry_t* otherObject);

bool_t
Line_crosses(Line_t* self, Geometry_t* otherObject);

bool_t
Line_within(Line_t* self, Geometry_t* otherObject);

bool_t
Line_contains(Line_t* self, Geometry_t* otherObject);

bool_t
Line_overlaps(Line_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_LINE_H_ */
