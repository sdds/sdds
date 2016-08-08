/*
 * Geometry_Point.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_

#include "Geometry.h"

bool_t
Point_equals(Point_t* self, Geometry_t* otherObject);

bool_t
Point_disjoint(Point_t* self, Geometry_t* otherObject);

bool_t
Point_intersects(Point_t* self, Geometry_t* otherObject);

bool_t
Point_touches(Point_t* self, Geometry_t* otherObject);

bool_t
Point_crosses(Point_t* self, Geometry_t* otherObject);

bool_t
Point_within(Point_t* self, Geometry_t* otherObject);

bool_t
Point_contains(Point_t* self, Geometry_t* otherObject);

bool_t
Point_overlaps(Point_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_ */
