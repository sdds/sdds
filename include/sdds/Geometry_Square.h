/*
 * Geometry_Square.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_

#include "Geometry.h"

bool_t
Square_equals(Square_t* self, Geometry_t* otherObject);

bool_t
Square_disjoint(Square_t* self, Geometry_t* otherObject);

bool_t
Square_intersects(Square_t* self, Geometry_t* otherObject);

bool_t
Square_touches(Square_t* self, Geometry_t* otherObject);

bool_t
Square_crosses(Square_t* self, Geometry_t* otherObject);

bool_t
Square_within(Square_t* self, Geometry_t* otherObject);

bool_t
Square_contains(Square_t* self, Geometry_t* otherObject);

bool_t
Square_overlaps(Square_t* self, Geometry_t* otherObject);

Polygon_t*
Square_asPolygon(Square_t* self);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_ */
