/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _PLANE_H
#define _PLANE_H

#include <QPainter>
#include <vector>

#include "Point.h"
#include "Constants.h"
#include "SystemTopologyData.h"

#define XX 0
#define YY 1
#define ZZ 2

//instances of this class represent a plane of the 3D topology

class Plane
{
public:

    //constructor
    Plane();

    //rotate the plane about the X-axis
    void
    xRotate( double angle );

    //rotate the plane about the Y-axis
    void
    yRotate( double angle );

    //rotate the plane about the Z-axis
    void
    zRotate( double angle );

    //scroll the plane along the X-axis
    void
    xScroll( double value );

    //scroll the plane long the Y-axis
    void
    yScroll( double value );

    //scroll the plane along the Z-axis
    void
    zScroll( double value );

    //scale the distance from the argument point (x,y,z)
    //to the plane's points by the parameter factor
    bool
    scale( double x,
           double y,
           double z,
           double factor );

    void
    scale( double factor );

    //in the "bool* visible" array we store which positions on the planes
    //would not be hidden by another plane above it;
    //only those have to be painted on non-top planes;
    //this method computes the values of this array;
    //parameters:
    //dim[]         are the original dimensions (size = 3)
    //dimOrder[]    defines the order of dimensions (size = 3),
    //              for example in XYZ-view it's elements are 0,1,2
    //              and in the YZX-view 1,2,0
    //planeDistance gives the current vertical distance between planes in 3D-topologies
    void
    computeVisibles( const SystemTopologyData* data,
                     int                       planeDistance );


    //this method paints the plane;
    //parameters:
    //dim[]             are the original dimensions (size = 3)
    //dimOrder[]        defines the order of dimensions (size = 3),
    //                  for example in XYZ-view it's elements are 0,1,2
    //                  and in the YZX-view 1,2,0
    //currentPlaneIndex is the index in the first dimension, needed to get the colors
    //painter           is the QPainter to paint with
    //colors            is a block of size dim[0] x dim[1] x dim[2] storing the colors
    //                  for the topology's system items
    //selected_rects    is a block of size dim[0] x dim[1] x dim[2] storing the status, if the selected item belongs to selected threads

    //isPlaneAbove      tells if there is another plane above this; if yes then
    //                  we don't draw the items that are covered by that top plane
    //lastPoint         is the point where a mouse selection happened if any (otherwise NULL)
    //dim1Index and dim2Index :
    //                  we put in here the position of the item that was clicked (only if lastPoint is not NULL)
    //                  note that these values are returned back for further computation in the SystemTopologyWidget class
    void
    paint( SystemTopologyData* data,
           unsigned            currentPlaneIndex,
           QPainter &          painter,
           bool                isPlaneAbove,
           QPoint*             lastPoint,
           int &               dim1Index,
           int &               dim2Index );

    //returns the corner points of a rectangle around the plane
    void
    getCornerPoints( double& minX,
                     double& maxX,
                     double& minY,
                     double& maxY );

    //returns the most-top-most-left point of the plane
    void
    getTopLeftPoint( double& x,
                     double& y,
                     double& z );

    //returns the index-th element of the points array
    const Point*
    getPoint( unsigned index );
    void
    setPoint( unsigned     index,
              const Point &p );

    //determines if the front of the plane is lower then the back part,
    //in this case the deepest plane will be painted first,
    //otherwise we start with the one on the top, because we see the block from the bottom
    bool
    isRising();

    //sets the line type;
    //LineType is defined in constants.h;
    //lineType stores if the lines between the system items are black, gray,
    //white or if no lines should be drawn
    void
    setLineType( LineType lineType );

    void
    setFoldingSeparator( const unsigned dims[] = 0 );

private:
    //mirror the plane on the X-axis
    void
    xMirror();

    //mirror the plane on the Y-axis
    void
    yMirror();

    //mirror the plane on the Z-axis
    void
    zMirror();

    //points[0]..points[3] are the corner points;
    //points[4] is the middle point;
    Point points[ 5 ];
    // values of points, but each element of each dimension of the topology is rounded to int
    Point pointsOnScreen[ 5 ];

    //in the visible array we store which positions on the planes
    //would not be hidden by another plane above it;
    //only those have to be painted on non-top planes
    bool* visible;

    //LineType is defined in constants.h;
    //lineType stores if the lines between the system items are black, gray, white or no lines
    LineType lineType;

    bool     markMerged; // draw rectangle around merged elements, if true
    int      foldingDimensions[ 3 ];
};

#endif
