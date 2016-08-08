/*
 * Geometry_SquareExtrusion.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_SQUAREEXTRUSION_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_SQUAREEXTRUSION_H_

#include "Geometry.h"

bool_t
SquareExtrusion_equals(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_disjoint(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_intersects(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_touches(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_crosses(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_within(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_contains(SquareExtrusion_t* self, Geometry_t* otherObject);

bool_t
SquareExtrusion_overlaps(SquareExtrusion_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_SQUAREEXTRUSION_H_ */
