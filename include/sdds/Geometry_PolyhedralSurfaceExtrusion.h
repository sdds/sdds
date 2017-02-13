/*
 * Geometry_PolyhedralSurfaceExtrusion.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_

#include "Geometry.h"

bool_t
PolyhedralSurfaceExtrusion_equals(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_disjoint(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_intersects(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_touches(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_crosses(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_within(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_contains(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_overlaps(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_ */
