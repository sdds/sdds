/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      Geometry_EllipseExtrusion.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
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
