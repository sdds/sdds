/*
 * Geometry_PolyhedralSurface.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_

#include "Geometry.h"

bool_t
PolyhedralSurface_equals(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_disjoint(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_intersects(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_touches(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_crosses(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_within(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_contains(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_overlaps(PolyhedralSurface_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_ */
