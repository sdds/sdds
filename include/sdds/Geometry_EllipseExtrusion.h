/*
 * Geometry_EllipseExtrusion.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSEEXTRUSION_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSEEXTRUSION_H_

#include "Geometry.h"

bool_t
EllipseExtrusion_equals(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_disjoint(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_intersects(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_touches(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_crosses(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_within(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_contains(EllipseExtrusion_t* self, Geometry_t* otherObject);

bool_t
EllipseExtrusion_overlaps(EllipseExtrusion_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSEEXTRUSION_H_ */
