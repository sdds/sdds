/*
 * Geometry_Line.c
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "Geometry_Line.h"

#ifdef FEATURE_SDDS_GEOMETRY_LINE_ENABLED

static bool_t
s_Point_equals(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_disjoint(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_intersects(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_touches(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_crosses(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_within(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_contains(Line_t* self, Point_t* otherObject);
static bool_t
s_Point_overlaps(Line_t* self, Point_t* otherObject);

static bool_t
s_LineString_equals(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_disjoint(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_intersects(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_touches(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_crosses(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_within(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_contains(Line_t* self, LineString_t* otherObject);
static bool_t
s_LineString_overlaps(Line_t* self, LineString_t* otherObject);

static bool_t
s_Line_equals(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_disjoint(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_intersects(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_touches(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_crosses(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_within(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_contains(Line_t* self, Line_t* otherObject);
static bool_t
s_Line_overlaps(Line_t* self, Line_t* otherObject);

static bool_t
s_LineRing_equals(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_disjoint(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_intersects(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_touches(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_crosses(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_within(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_contains(Line_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_overlaps(Line_t* self, LineRing_t* otherObject);

static bool_t
s_Polygon_equals(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_disjoint(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_intersects(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_touches(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_crosses(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_within(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_contains(Line_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_overlaps(Line_t* self, Polygon_t* otherObject);

static bool_t
s_Ellipse_equals(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_disjoint(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_intersects(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_touches(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_crosses(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_within(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_contains(Line_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_overlaps(Line_t* self, Ellipse_t* otherObject);

static bool_t
s_Square_equals(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_disjoint(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_intersects(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_touches(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_crosses(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_within(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_contains(Line_t* self, Square_t* otherObject);
static bool_t
s_Square_overlaps(Line_t* self, Square_t* otherObject);

static bool_t
s_PolyhedralSurface_equals(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_disjoint(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_intersects(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_touches(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_crosses(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_within(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_contains(Line_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_overlaps(Line_t* self, PolyhedralSurface_t* otherObject);

static bool_t
s_EllipseExtrusion_equals(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_disjoint(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_intersects(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_touches(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_crosses(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_within(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_contains(Line_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_overlaps(Line_t* self, EllipseExtrusion_t* otherObject);

static bool_t
s_SquareExtrusion_equals(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_disjoint(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_intersects(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_touches(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_crosses(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_within(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_contains(Line_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_overlaps(Line_t* self, SquareExtrusion_t* otherObject);

static bool_t
s_PolygonExtrusion_equals(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_disjoint(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_intersects(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_touches(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_crosses(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_within(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_contains(Line_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_overlaps(Line_t* self, PolygonExtrusion_t* otherObject);

static bool_t
s_PolyhedralSurfaceExtrusion_equals(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_intersects(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_touches(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_crosses(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_within(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_contains(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject);

/******************************************************
 *                  Implementation
 ******************************************************/

bool_t
Line_equals(Line_t* self, Geometry_t* otherObject) {
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
Line_disjoint(Line_t* self, Geometry_t* otherObject) {
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
Line_intersects(Line_t* self, Geometry_t* otherObject) {
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
Line_touches(Line_t* self, Geometry_t* otherObject) {
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
Line_crosses(Line_t* self, Geometry_t* otherObject) {
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
Line_within(Line_t* self, Geometry_t* otherObject) {
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
Line_contains(Line_t* self, Geometry_t* otherObject) {
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
Line_overlaps(Line_t* self, Geometry_t* otherObject) {
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
s_Point_equals(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_disjoint(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_intersects(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_touches(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_crosses(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_within(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_contains(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_overlaps(Line_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_LineString_equals(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_disjoint(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_intersects(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_touches(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_crosses(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_within(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_contains(Line_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_overlaps(Line_t* self, LineString_t* otherObject) {
    return false;
}


static bool_t
s_Line_equals(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_disjoint(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_intersects(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_touches(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_crosses(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_within(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_contains(Line_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_overlaps(Line_t* self, Line_t* otherObject) {
    return false;
}


static bool_t
s_LineRing_equals(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_disjoint(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_intersects(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_touches(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_crosses(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_within(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_contains(Line_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_overlaps(Line_t* self, LineRing_t* otherObject) {
    return false;
}


static bool_t
s_Polygon_equals(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_disjoint(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_intersects(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_touches(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_crosses(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_within(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_contains(Line_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_overlaps(Line_t* self, Polygon_t* otherObject) {
    return false;
}


static bool_t
s_Ellipse_equals(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_disjoint(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_intersects(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_touches(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_crosses(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_within(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_contains(Line_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_overlaps(Line_t* self, Ellipse_t* otherObject) {
    return false;
}


static bool_t
s_Square_equals(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_disjoint(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_intersects(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_touches(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_crosses(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_within(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_contains(Line_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_overlaps(Line_t* self, Square_t* otherObject) {
    return false;
}


static bool_t
s_PolyhedralSurface_equals(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_disjoint(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_intersects(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_touches(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_crosses(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_within(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_contains(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_overlaps(Line_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}


static bool_t
s_EllipseExtrusion_equals(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_disjoint(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_intersects(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_touches(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_crosses(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_within(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_contains(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_overlaps(Line_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_equals(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_disjoint(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_intersects(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_touches(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_crosses(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_within(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_contains(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_overlaps(Line_t* self, SquareExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolygonExtrusion_equals(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_disjoint(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_intersects(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_touches(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_crosses(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_within(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_contains(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_overlaps(Line_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolyhedralSurfaceExtrusion_equals(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_intersects(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_touches(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_crosses(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_within(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_contains(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(Line_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

#endif
