/*
 * Geometry.c
 *
 *  Created on: Aug 1, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "Geometry.h"
#include "Geometry_Point.h"
#include "Geometry_LineString.h"
#include "Geometry_Line.h"
#include "Geometry_LineRing.h"
#include "Geometry_Polygon.h"
#include "Geometry_Ellipse.h"
#include "Geometry_Square.h"
#include "Geometry_PolyhedralSurface.h"
#include "Geometry_EllipseExtrusion.h"
#include "Geometry_SquareExtrusion.h"
#include "Geometry_PolygonExtrusion.h"
#include "Geometry_PolyhedralSurfaceExtrusion.h"

#ifdef FEATURE_SDDS_GEOMETRY_ENABLED

bool_t
Geometry_eaquals(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);

    switch (self->type) {
    case GEO_TYPE_POINT:
        return Point_equals((Point_t*)self, otherObject);

#ifdef FEATURE_SDDS_GEOMETRY_LINESTRING_ENABLED
    case GEO_TYPE_LINESTRING:
        return LineString_equals((LineString_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINE_ENABLED
    case GEO_TYPE_LINE:
        return Line_equals((Line_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINERING_ENABLED
    case GEO_TYPE_LINERING:
        return LineRing_equals((LineRing_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_ENABLED
    case GEO_TYPE_ELLIPSE:
        return Ellipse_equals((Ellipse_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_ENABLED
    case GEO_TYPE_SQUARE:
        return Square_equals((Square_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_ENABLED
    case GEO_TYPE_POLYGON:
        return Polygon_equals((Polygon_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_ENABLED
    case GEO_TYPE_POLYHEDRALSURFACE:
        return PolyhedralSurface_equals((PolyhedralSurface_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_EXTRUSION_ENABLED
    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return EllipseExtrusion_equals((EllipseExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_EXTRUSION_ENABLED
    case GEO_TYPE_SQUARE_EXTRUSION:
        return SquareExtrusion_equals((SquareExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_EXTRUSION_ENABLED
    case GEO_TYPE_POLYGON_EXTRUSION:
        return PolygonExtrusion_equals((PolygonExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_EXTRUSION_ENABLED
    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return PolyhedralSurfaceExtrusion_equals((PolyhedralSurfaceExtrusion_t*)self, otherObject);
#endif

    default:
        return false;
    }
}

bool_t
Geometry_disjoint(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);
    return false;
}

bool_t
Geometry_intersects(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);
    return false;
}

bool_t
Geometry_touches(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);
    return false;
}

bool_t
Geometry_crosses(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);
    return false;
}

bool_t
Geometry_within(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);

    switch (self->type) {
    case GEO_TYPE_POINT:
        return Point_within((Point_t*)self, otherObject);

#ifdef FEATURE_SDDS_GEOMETRY_LINESTRING_ENABLED
    case GEO_TYPE_LINESTRING:
        return LineString_within((LineString_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINE_ENABLED
    case GEO_TYPE_LINE:
        return Line_within((Line_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINERING_ENABLED
    case GEO_TYPE_LINERING:
        return LineRing_within((LineRing_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_ENABLED
    case GEO_TYPE_ELLIPSE:
        return Ellipse_within((Ellipse_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_ENABLED
    case GEO_TYPE_SQUARE:
        return Square_within((Square_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_ENABLED
    case GEO_TYPE_POLYGON:
        return Polygon_within((Polygon_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_ENABLED
    case GEO_TYPE_POLYHEDRALSURFACE:
        return PolyhedralSurface_within((PolyhedralSurface_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_EXTRUSION_ENABLED
    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return EllipseExtrusion_within((EllipseExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_EXTRUSION_ENABLED
    case GEO_TYPE_SQUARE_EXTRUSION:
        return SquareExtrusion_within((SquareExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_EXTRUSION_ENABLED
    case GEO_TYPE_POLYGON_EXTRUSION:
        return PolygonExtrusion_within((PolygonExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_EXTRUSION_ENABLED
    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return PolyhedralSurfaceExtrusion_within((PolyhedralSurfaceExtrusion_t*)self, otherObject);
#endif

    default:
        return false;
    }
}

bool_t
Geometry_contains(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);
    return false;
}

bool_t
Geometry_overlaps(Geometry_t* self, Geometry_t* otherObject) {
    assert(self);
    assert(otherObject);

    switch (self->type) {
     case GEO_TYPE_POINT:
         return Point_overlaps((Point_t*)self, otherObject);

#ifdef FEATURE_SDDS_GEOMETRY_LINESTRING_ENABLED
     case GEO_TYPE_LINESTRING:
         return LineString_overlaps((LineString_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINE_ENABLED
     case GEO_TYPE_LINE:
         return Line_overlaps((Line_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_LINERING_ENABLED
     case GEO_TYPE_LINERING:
         return LineRing_overlaps((LineRing_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_ENABLED
     case GEO_TYPE_ELLIPSE:
         return Ellipse_overlaps((Ellipse_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_ENABLED
     case GEO_TYPE_SQUARE:
         return Square_overlaps((Square_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_ENABLED
     case GEO_TYPE_POLYGON:
         return Polygon_overlaps((Polygon_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_ENABLED
     case GEO_TYPE_POLYHEDRALSURFACE:
         return PolyhedralSurface_overlaps((PolyhedralSurface_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_ELLIPSE_EXTRUSION_ENABLED
     case GEO_TYPE_ELLIPSE_EXTRUSION:
         return EllipseExtrusion_overlaps((EllipseExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_SQUARE_EXTRUSION_ENABLED
     case GEO_TYPE_SQUARE_EXTRUSION:
         return SquareExtrusion_overlaps((SquareExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYGON_EXTRUSION_ENABLED
     case GEO_TYPE_POLYGON_EXTRUSION:
         return PolygonExtrusion_overlaps((PolygonExtrusion_t*)self, otherObject);
#endif

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_EXTRUSION_ENABLED
     case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
         return PolyhedralSurfaceExtrusion_overlaps((PolyhedralSurfaceExtrusion_t*)self, otherObject);
#endif

     default:
         return false;
     }
}

Polygon_t*
BasicShape_asPolygon(BasicShape_t* self)  {
    return NULL;
}

void
Geometry_initPoint(Point_t* self) {
    assert(self);
    self->geometry.type = GEO_TYPE_POINT;
    self->x = 0;
    self->y = 0;
    self->z = 0;
}

void
Geometry_initSquare(Square_t* self) {
    assert(self);
    self->basicShape.geometry.type = GEO_TYPE_SQUARE;
    Geometry_initPoint(&self->basicShape.vertex);
}

void
Geometry_initEllipse(Ellipse_t* self) {
    assert(self);
    self->basicShape.geometry.type = GEO_TYPE_ELLIPSE;
    Geometry_initPoint(&self->basicShape.vertex);
}

#endif
