/*
 * Geometry_LineRing.h
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_LINERING_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_LINERING_H_

#include "Geometry.h"

bool_t
LineRing_equals(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_disjoint(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_intersects(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_touches(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_crosses(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_within(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_contains(LineRing_t* self, Geometry_t* otherObject);

bool_t
LineRing_overlaps(LineRing_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_LINERING_H_ */
