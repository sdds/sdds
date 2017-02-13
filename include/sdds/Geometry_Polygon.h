/*
 * Geometry_Polygon.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYGON_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYGON_H_

#include "Geometry.h"

bool_t
Polygon_equals(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_disjoint(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_intersects(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_touches(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_crosses(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_within(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_contains(Polygon_t* self, Geometry_t* otherObject);

bool_t
Polygon_overlaps(Polygon_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYGON_H_ */
