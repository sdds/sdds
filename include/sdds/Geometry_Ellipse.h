/*
 * Geometry_Ellipse.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_

#include "Geometry.h"

bool_t
Ellipse_equals(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_disjoint(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_intersects(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_touches(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_crosses(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_within(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_contains(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_overlaps(Ellipse_t* self, Geometry_t* otherObject);

Polygon_t*
Ellipse_asPolygon(Ellipse_t* self);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_ */
