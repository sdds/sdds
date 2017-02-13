/*
 * Geometry_PolygonExtrusion.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYGONEXTRUSION_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYGONEXTRUSION_H_

#include "Geometry.h"

bool_t
PolygonExtrusion_equals(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_disjoint(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_intersects(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_touches(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_crosses(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_within(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_contains(PolygonExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolygonExtrusion_overlaps(PolygonExtrusion_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYGONEXTRUSION_H_ */
