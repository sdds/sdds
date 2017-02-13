/*
 * Geometry_PolyhedralSurface.c
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "Geometry_PolyhedralSurface.h"

#ifdef FEATURE_SDDS_GEOMETRY_POLYHEDRALSURFACE_ENABLED

static bool_t
s_Point_equals(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_disjoint(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_intersects(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_touches(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_crosses(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_within(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_contains(PolyhedralSurface_t* self, Point_t* otherObject);
static bool_t
s_Point_overlaps(PolyhedralSurface_t* self, Point_t* otherObject);

static bool_t
s_LineString_equals(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_disjoint(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_intersects(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_touches(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_crosses(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_within(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_contains(PolyhedralSurface_t* self, LineString_t* otherObject);
static bool_t
s_LineString_overlaps(PolyhedralSurface_t* self, LineString_t* otherObject);

static bool_t
s_Line_equals(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_disjoint(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_intersects(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_touches(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_crosses(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_within(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_contains(PolyhedralSurface_t* self, Line_t* otherObject);
static bool_t
s_Line_overlaps(PolyhedralSurface_t* self, Line_t* otherObject);

static bool_t
s_LineRing_equals(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_disjoint(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_intersects(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_touches(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_crosses(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_within(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_contains(PolyhedralSurface_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_overlaps(PolyhedralSurface_t* self, LineRing_t* otherObject);

static bool_t
s_Polygon_equals(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_disjoint(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_intersects(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_touches(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_crosses(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_within(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_contains(PolyhedralSurface_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_overlaps(PolyhedralSurface_t* self, Polygon_t* otherObject);

static bool_t
s_Ellipse_equals(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_disjoint(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_intersects(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_touches(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_crosses(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_within(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_contains(PolyhedralSurface_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_overlaps(PolyhedralSurface_t* self, Ellipse_t* otherObject);

static bool_t
s_Square_equals(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_disjoint(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_intersects(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_touches(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_crosses(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_within(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_contains(PolyhedralSurface_t* self, Square_t* otherObject);
static bool_t
s_Square_overlaps(PolyhedralSurface_t* self, Square_t* otherObject);

static bool_t
s_PolyhedralSurface_equals(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_disjoint(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_intersects(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_touches(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_crosses(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_within(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_contains(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_overlaps(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject);

static bool_t
s_EllipseExtrusion_equals(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_disjoint(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_intersects(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_touches(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_crosses(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_within(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_contains(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_overlaps(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject);

static bool_t
s_SquareExtrusion_equals(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_disjoint(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_intersects(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_touches(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_crosses(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_within(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_contains(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_overlaps(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject);

static bool_t
s_PolygonExtrusion_equals(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_disjoint(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_intersects(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_touches(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_crosses(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_within(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_contains(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_overlaps(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject);

static bool_t
s_PolyhedralSurfaceExtrusion_equals(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_intersects(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_touches(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_crosses(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_within(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_contains(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject);

/******************************************************
 *                  Implementation
 ******************************************************/

bool_t
PolyhedralSurface_equals(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_equals(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_equals(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_equals(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_equals(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_equals(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_equals(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_equals(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_equals(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_equals(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_equals(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_equals(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_equals(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_disjoint(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_disjoint(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_disjoint(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_disjoint(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_disjoint(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_disjoint(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_disjoint(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_disjoint(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_disjoint(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_disjoint(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_disjoint(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_disjoint(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_disjoint(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_intersects(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_intersects(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_intersects(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_intersects(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_intersects(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_intersects(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_intersects(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_intersects(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_intersects(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_intersects(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_intersects(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_intersects(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_intersects(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_touches(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_touches(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_touches(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_touches(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_touches(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_touches(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_touches(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_touches(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_touches(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_touches(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_touches(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_touches(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_touches(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_crosses(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_crosses(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_crosses(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_crosses(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_crosses(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_crosses(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_crosses(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_crosses(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_crosses(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_crosses(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_crosses(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_crosses(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_crosses(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_within(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_within(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_within(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_within(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_within(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_within(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_within(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_within(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_within(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_within(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_within(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_within(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_within(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_contains(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_contains(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_contains(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_contains(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_contains(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_contains(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_contains(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_contains(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_contains(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_contains(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_contains(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_contains(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_contains(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
PolyhedralSurface_overlaps(PolyhedralSurface_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_overlaps(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_overlaps(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_overlaps(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_overlaps(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_overlaps(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_overlaps(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_overlaps(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_overlaps(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_overlaps(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_overlaps(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_overlaps(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_overlaps(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}


/******************************************************
 *          Static Functions Implementation
 ******************************************************/

static bool_t
s_Point_equals(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_disjoint(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_intersects(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_touches(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_crosses(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_within(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_contains(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_overlaps(PolyhedralSurface_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_LineString_equals(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_disjoint(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_intersects(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_touches(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_crosses(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_within(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_contains(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_overlaps(PolyhedralSurface_t* self, LineString_t* otherObject) {
    return false;
}


static bool_t
s_Line_equals(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_disjoint(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_intersects(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_touches(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_crosses(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_within(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_contains(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_overlaps(PolyhedralSurface_t* self, Line_t* otherObject) {
    return false;
}


static bool_t
s_LineRing_equals(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_disjoint(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_intersects(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_touches(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_crosses(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_within(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_contains(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_overlaps(PolyhedralSurface_t* self, LineRing_t* otherObject) {
    return false;
}


static bool_t
s_Polygon_equals(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_disjoint(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_intersects(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_touches(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_crosses(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_within(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_contains(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_overlaps(PolyhedralSurface_t* self, Polygon_t* otherObject) {
    return false;
}


static bool_t
s_Ellipse_equals(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_disjoint(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_intersects(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_touches(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_crosses(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_within(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_contains(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_overlaps(PolyhedralSurface_t* self, Ellipse_t* otherObject) {
    return false;
}


static bool_t
s_Square_equals(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_disjoint(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_intersects(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_touches(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_crosses(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_within(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_contains(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_overlaps(PolyhedralSurface_t* self, Square_t* otherObject) {
    return false;
}


static bool_t
s_PolyhedralSurface_equals(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_disjoint(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_intersects(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_touches(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_crosses(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_within(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_contains(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_overlaps(PolyhedralSurface_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}


static bool_t
s_EllipseExtrusion_equals(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_disjoint(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_intersects(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_touches(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_crosses(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_within(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_contains(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_overlaps(PolyhedralSurface_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_SquareExtrusion_equals(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_disjoint(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_intersects(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_touches(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_crosses(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_within(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_contains(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_overlaps(PolyhedralSurface_t* self, SquareExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolygonExtrusion_equals(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_disjoint(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_intersects(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_touches(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_crosses(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_within(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_contains(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_overlaps(PolyhedralSurface_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolyhedralSurfaceExtrusion_equals(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_intersects(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_touches(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_crosses(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_within(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_contains(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(PolyhedralSurface_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

#endif
