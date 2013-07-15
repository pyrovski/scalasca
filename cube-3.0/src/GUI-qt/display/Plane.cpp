/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2012                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <QDebug>
#include <cassert>
#include <cmath>
#include <iostream>

#include "Plane.h"
#include "Tetragon.h"
using namespace std;


//constructor
Plane::Plane()
{
    visible    = NULL;
    markMerged = false;
}

//rotate the plane about the X-axis
void
Plane::xRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].xRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
}

//rotate the plane about the Y-axis
void
Plane::yRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].yRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
}

//rotate the plane about the Z-axis
void
Plane::zRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].zRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
}

//scroll the plane along the X-axis
void
Plane::xScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].xScroll( value );
        pointsOnScreen[ i ].xScroll( value );
    }
}

//scroll the plane along the Y-axis
void
Plane::yScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].yScroll( value );
        pointsOnScreen[ i ].yScroll( value );
    }
}

//scroll the plane along the Z-axis
void
Plane::zScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].zScroll( value );
        pointsOnScreen[ i ].zScroll( value );
    }
}

//mirror the plane on the X-axis
void
Plane::xMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].xMirror();
    }
}

//mirror the plane on the Y-axis
void
Plane::yMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].yMirror();
    }
}

//mirror the plane on the Z-axis
void
Plane::zMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].zMirror();
    }
}


//scale(x,y,z,factor) scales the distance from the argument point (x,y,z)
//to the plane's points by the parameter factor
//
bool
Plane::scale( double x, double y, double z, double factor )
{
    bool ok = true;
    //make smaller only if the plane has some minimal size
    if ( factor < 1 )
    {
        double xMin, xMax, yMin, yMax;
        getCornerPoints( xMin, xMax, yMin, yMax );
        double diff = ( xMax - xMin ) + ( yMax - yMin );
        ok = ( diff >= 10 );
    }
    if ( ok )
    {
        for ( unsigned i = 0; i < 5; i++ )
        {
            points[ i ].scale( x, y, z, factor );
            pointsOnScreen[ i ].scale( x, y, z, factor );
        }
    }
    return ok;
}

void
Plane::scale( double factor )
{
    double x, y, z;
    getTopLeftPoint( x, y, z );
    xScroll( -x );
    yScroll( -y );
    scale( 0, 0, 0, factor );
    xScroll( TOPOLOGY_MARGIN );
    yScroll( TOPOLOGY_MARGIN );
}

//returns the index-th element of the points array;
//in the points array points[0]..points[3] are the corner points;
//points[4] is the middle point;
//
const Point*
Plane::getPoint( unsigned index )
{
    assert( index < 5 );
    return &( pointsOnScreen[ index ] );
}

void
Plane::setPoint( unsigned index, const Point &p )
{
    assert( index < 5 );
    points[ index ]         = p;
    pointsOnScreen[ index ] = p;
}

//in the "bool* visible" array we store which positions on the planes
//would not be hidden by another plane above it;
//only those have to be painted on non-top planes;
//this method computes the values of this array;
//parameters:
//dim[]         : are the original dimensions (size = 3)
//dimOrder[]    : defines the order of dimensions (size = 3),
//                for example in XYZ-view it's elements are 0,1,2
//                and in the YZX-view 1,2,0
//planeDistance : gives the current vertical distance between planes in 3D-topologies
//
void
Plane::computeVisibles( const SystemTopologyData* data, int planeDistance )
{
    //if empty topology then return
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }

    //clean up the visible array
    if ( visible != NULL )
    {
        delete[] visible;
    }
    visible = new bool[ ( int )( data->getDim( XX ) * data->getDim( YY ) ) ];
    for ( int i = 0; i < ( int )( data->getDim( XX ) * data->getDim( YY ) ); i++ )
    {
        visible[ i ] = true;
    }

    if ( data->getDim( ZZ ) <= 1 )
    {
        return;
    }

    /*        (dx01,dy01)
     *             -->
     *  points[0]  --------------------points[1]
     *            |  |  |  |  |  |  |  |
     *            |---------------------
     * (dx03,dy03)|  |  |  |  |  |  |  |
     *          V |---------------------
     *            |  |  |  |  |  |  |  |
     *            |---------------------
     *            |  |  |  |  |  |  |  |
     *  points[3]  --------------------points[2]
     */

    int dx01 = ( int )( ( points[ 1 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( XX ) );
    int dy01 = ( int )( ( points[ 1 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( XX ) );
    int dx03 = ( int )( ( points[ 3 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( YY ) );
    int dy03 = ( int )( ( points[ 3 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( YY ) );

    //(x,y) is poinst[0]
    int x = ( int )( points[ 0 ].getX() );
    int y = ( int )( points[ 0 ].getY() );

    //quad is the first small square starting at points[0]
    Tetragon quad;
    quad.push_back( QPointF( ( double )( x ),               ( double )( y ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 ),        ( double )( y + dy01 ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 + dx03 ), ( double )( y + dy01 + dy03 ) ) );
    quad.push_back( QPointF( ( double )( x + dx03 ),        ( double )( y + dy03 ) ) );

    //topPlane is the plane above the current one
    Tetragon topPlane;
    y = ( int )( points[ 0 ].getY() + planeDistance );

    topPlane.push_back( QPointF( ( double )( x ),
                                 ( double )( y ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( XX ) * dx01 ),
                                 ( double )( y + ( int )data->getDim( XX ) * dy01 ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( XX ) * dx01 + ( int )data->getDim( YY ) * dx03 ),
                                 ( double )( y + ( int )data->getDim( XX ) * dy01 + ( int )data->getDim( YY ) * dy03 ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( YY ) * dx03 ),
                                 ( double )( y + ( int )data->getDim( YY ) * dy03 ) ) );

    int index = 0;

    for ( int i = 0; i < ( int )data->getDim( XX ); i++ )
    {
        for ( int j = 0; j < ( int )data->getDim( YY ); j++, index++ )
        {
            //if one of the small square's (quad's) corner is visible we will pain it
            visible[ index ] = ( !topPlane.containsPoint( quad.at( 0 ) ) ||
                                 !topPlane.containsPoint( quad.at( 1 ) ) ||
                                 !topPlane.containsPoint( quad.at( 2 ) ) ||
                                 !topPlane.containsPoint( quad.at( 3 ) ) );
            //take the next square in the 3rd dimension
            quad.translate( dx03, dy03 );
        }
        //check the next square in the 2nd dimension
        quad.translate( dx01 - ( int )( data->getDim( YY ) ) * dx03, dy01 - ( int )( data->getDim( YY ) ) * dy03 );
    }
}
//end of computeVisibles()

/**
 * If two dimensions are merged into one, a separator is drawn
   @param dims if dimension consists of 2 merged dimensions, dims contains the size
          of the 2nd merged dimension
 */
void
Plane::setFoldingSeparator( const unsigned dims[] )
{
    if ( dims != 0 )
    {
        markMerged = false;
        for ( int i = 0; i < 3; i++ )
        {
            foldingDimensions[ i ] = dims[ i ];
            if ( dims[ i ] > 1 )
            {
                markMerged = true;                // dimensions to merge exists
            }
        }
    }
    else
    {
        markMerged = false;
    }
}

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
//isPlaneAbove      tells if there is another plane above this; if yes then
//                  we don't draw the items that are covered by that top plane
//lastPoint         is the point where a mouse selection happened if any (otherwise NULL)
//dim1Index and dim2Index
//                  we put in here the position of the item that was clicked (only if lastPoint is not NULL)
//                  note that these values are returned back for further computation in the SystemTopologyWidget class

void
Plane::paint( SystemTopologyData* data,
              unsigned            currentPlaneIndex,
              QPainter&           painter,
              bool                isPlaneAbove,
              QPoint*             lastPoint,
              int &               dim1Index,
              int &               dim2Index )
{
    //return if the topology is empty
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }
    //----------------------------------------------
    if ( !data->hasUniqueIds() )
    {
        markMerged = false;
    }
    QVector<QLineF> selectedArea;
    //----------------------------------------------

    QColor innerColor;
    QColor outerColor;

    if ( lineType == BLACK_LINES )
    {
        outerColor = Qt::black;
    }
    else if ( lineType == GRAY_LINES )
    {
        outerColor = Qt::gray;
    }
    else if ( lineType == WHITE_LINES )
    {
        outerColor = Qt::white;
    }
    if ( markMerged )
    {
        if ( lineType == BLACK_LINES )
        {
            innerColor = Qt::gray;
        }
        else if ( lineType == GRAY_LINES )
        {
            innerColor = Qt::black;
        }
        else if ( lineType == WHITE_LINES )
        {
            innerColor = Qt::gray;
        }
    }
    else
    {
        innerColor = outerColor;
    }

    //pen1 is the standard pen for painting
    QPen pen1;
    pen1.setWidth( 0 );
    pen1.setJoinStyle( Qt::MiterJoin );
    pen1.setMiterLimit( 0 );
    pen1.setColor( innerColor );
    painter.setPen( pen1 );

    //pen2 is for painting the first selected item
    QPen pen2( Qt::magenta );
    pen2.setWidth( 3 );
    pen2.setJoinStyle( Qt::MiterJoin );
    pen2.setMiterLimit( 0 );

    //dx01, dy01, dx03, dy03 are as explained in method computeVisibles
    int dx01 = ( int )( ( points[ 1 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( XX ) );
    int dy01 = ( int )( ( points[ 1 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( XX ) );
    int dx03 = ( int )( ( points[ 3 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( YY ) );
    int dy03 = ( int )( ( points[ 3 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( YY ) );

    // adjust the corner points
    // The single elements are drawn with integer sizes. So the plane outline has to be adjusted
    // to fit multiples of a single element
    pointsOnScreen[ 0 ].setX( points[ 0 ].getX() );
    pointsOnScreen[ 0 ].setY( points[ 0 ].getY() );
    pointsOnScreen[ 1 ].setX( points[ 0 ].getX() + dx01 * ( int )data->getDim( XX ) );
    pointsOnScreen[ 1 ].setY( points[ 0 ].getY() + dy01 * ( int )data->getDim( XX ) );
    pointsOnScreen[ 3 ].setX( points[ 0 ].getX() + dx03 * ( int )data->getDim( YY ) );
    pointsOnScreen[ 3 ].setY( points[ 0 ].getY() + dy03 * ( int )data->getDim( YY ) );
    pointsOnScreen[ 2 ].setX( pointsOnScreen[ 3 ].getX() + dx01 * ( int )data->getDim( XX ) );
    pointsOnScreen[ 2 ].setY( pointsOnScreen[ 3 ].getY() + dy01 * ( int )data->getDim( XX ) );

    //(x,y) is points[0]
    int x = ( int )points[ 0 ].getX();
    int y = ( int )points[ 0 ].getY();

    //quad is the first small square starting at points[0]
    Tetragon quad;
    quad.push_back( QPointF( ( double )( x ),               ( double )( y ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 ),        ( double )( y + dy01 ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 + dx03 ), ( double )( y + dy01 + dy03 ) ) );
    quad.push_back( QPointF( ( double )( x + dx03 ),        ( double )( y + dy03 ) ) );

    //found stores if we already found the selected item;
    //this is necessary, because if the user clicks on the line between two items, then
    //the selection is not unique;
    //in this case we select the item found first
    bool found = false;
    //index is for iteration in the visible array
    int  index = 0;
    //basically, we paint quad (the small square) and shift it further;
    //however, if it is not visible, we don't need to handle (shift and paint) it;
    //in skipped we count how many squares we did not paint (and thus did not shift quad),
    //thus before the next paint we can do shifting in one step
    int skipped = 0;
    //in shifted we count how often we shifted quad in a row of the second dimension;
    //before moving to the next row, we shift accordingly back to the beginning
    int       shifted = 0;

    QPolygonF singlePoly;
    for ( int i = 0; i < quad.size(); i++ )
    {
        singlePoly.append( quad.at( i ) );
    }

    QPointF diff();
    int     dim1 = data->getDim( XX );
    int     dim2 = data->getDim( YY );
    for ( int k1 = 0; k1 < dim1; k1++ )
    {
        for ( int k2 = 0; k2 < dim2; k2++, index++ )
        {
            //paint small square only if visible
            if ( !isPlaneAbove ? true : visible[ index ] )
            {
                //postponed shift for non-painted squares
                quad.translate( skipped * dx03, skipped * dy03 );
                shifted += skipped;
                skipped  = 1;
                //paint
                QColor* color = data->getColor( k1, k2, currentPlaneIndex );
                painter.setBrush( *color );
                if ( lineType == NO_LINES )
                {
                    pen1.setColor( *color );
                    painter.setPen( pen1 );
                }
                if ( !data->hasUniqueIds() )
                {
                    /* multi process topology => don't draw lines for each element, but draw
                     * outline of each area consisting of multiple elements with same systemId */
                    int neighb = data->getNeighbors( k1, k2, currentPlaneIndex );
                    if ( neighb != 0 )
                    {
                        pen1.setColor( *color );
                        painter.setPen( pen1 );
                        painter.drawConvexPolygon( quad );
                        pen1.setColor( outerColor );
                        painter.setPen( pen1 );
                        for ( int i = 0; i < 4; i++ )   // draw Line if the neighbor has different systemID
                        {
                            bool insideArea = neighb & ( 1 << i );
                            if ( !insideArea )
                            {
                                if ( data->isSelected( k1, k2, currentPlaneIndex ) )
                                {
                                    selectedArea.append( QLineF( quad[ i ], quad[ ( i + 1 ) % 4 ] ) );
                                }
                                if ( lineType != NO_LINES )
                                {
                                    painter.drawLine( quad[ i ], quad[ ( i + 1 ) % 4 ] );
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( lineType != NO_LINES )
                        {
                            pen1.setColor( outerColor );
                            painter.setPen( pen1 );
                        }
                        painter.drawConvexPolygon( quad );
                    }
                } // has unique id
                else
                {
                    painter.drawConvexPolygon( quad );
                }
                //if the user made a selection by click and the corresponding item is not yet found
                if ( lastPoint != NULL && !found )
                {
                    //check if the clicked point is within the current square
                    if ( quad.containsPoint( QPointF( ( double )lastPoint->x(), ( double )lastPoint->y() ) ) )
                    {
                        //store the position in (dim1Index,dim2Index)
                        dim1Index = k1;
                        dim2Index = k2;
                        found     = true;
                    }
                }
            }
            else
            {
                skipped++;
            }
        }
        //prepare for next row:
        //shift one row further and undo shifting of previous row
        quad.translate( dx01 - shifted * dx03, dy01 - shifted * dy03 );
        skipped = 0;
        shifted = 0;
    }

    // mark merged elements by drawing separator lines
    if ( markMerged && ( lineType != NO_LINES ) )
    {
        painter.save();
        pen1.setColor( outerColor );
        pen1.setWidth( 1 );
        painter.setPen( pen1 );

        // draw horizontal
        int height = dy01 * dim1;
        int width  = dx01 * dim1;
        for ( int i = 0; i <= dim2; i += foldingDimensions[ YY ] )
        {
            int yy = y + i * dy03;
            int xx = x + i * dx03;
            painter.drawLine( xx, yy, xx + width, yy + height );
        }

        height = dy03 * dim2;
        width  = dx03 * dim2;
        for ( int i = 0; i <= dim1; i += foldingDimensions[ XX ] )
        {
            int yy = y + i * dy01;
            int xx = x + i * dx01;
            painter.drawLine( xx, yy, xx + width, yy + height );
        }
        painter.restore();
    }

    //if an item was marked, then mark the item and
    //on the 4 plane sides its position
    //such that the position is visible even if the plane is (partly) covered by another plane

    /*
                        __
       points[0] --------------------points[1]
     |  |  |  |  |  |  |  |
     |||||||||||||||||||--------------------
     |  |  |  |  |  |  |  |
     |||||||||||||||||||---------------------
     | |  |  |  |S |  |  |  | |
     |||||||||||||||||||---------------------
     |  |  |  |  |  |  |  |
       points[3] --------------------points[2]
                        __
     */

    for ( int k1 = 0; k1 < dim1; k1++ )
    {
        for ( int k2 = 0; k2 < dim2; k2++ )
        {
            if ( data->isSelected( k1, k2, currentPlaneIndex ) )
            {
                painter.setPen( pen2 );
                painter.setBrush( *data->getColor( k1, k2, currentPlaneIndex ) );

                int x0 = ( int )points[ 0 ].getX();
                int x1 = x0 + dx01;
                int x2 = x0 + dx01 + dx03;
                int x3 = x0 + dx03;

                int y0 = ( int )points[ 0 ].getY();
                int y1 = y0 + dy01;
                int y2 = y0 + dy01 + dy03;
                int y3 = y0 + dy03;

                //enlarge the square of the selected item such that the color inside will be
                //visible also if the square is small

                int middleX = ( x0 + x2 ) / 2;
                int middleY = ( y0 + y2 ) / 2;

                if ( middleX > x0 )
                {
                    x0 -= 2;
                }
                else
                {
                    x0 += 2;
                }
                if ( middleX > x1 )
                {
                    x1 -= 2;
                }
                else
                {
                    x1 += 2;
                }
                if ( middleX > x2 )
                {
                    x2 -= 2;
                }
                else
                {
                    x2 += 2;
                }
                if ( middleX > x3 )
                {
                    x3 -= 2;
                }
                else
                {
                    x3 += 2;
                }

                if ( middleY > y0 )
                {
                    y0 -= 2;
                }
                else
                {
                    y0 += 2;
                }
                if ( middleY > y1 )
                {
                    y1 -= 2;
                }
                else
                {
                    y1 += 2;
                }
                if ( middleY > y2 )
                {
                    y2 -= 2;
                }
                else
                {
                    y2 += 2;
                }
                if ( middleY > y3 )
                {
                    y3 -= 2;
                }
                else
                {
                    y3 += 2;
                }

                if ( selectedArea.size() == 0 )
                {
                    //paint the marked square for the selected item
                    quad.clear();
                    quad.push_back( QPoint( x0, y0 ) );
                    quad.push_back( QPoint( x1, y1 ) );
                    quad.push_back( QPoint( x2, y2 ) );
                    quad.push_back( QPoint( x3, y3 ) );
                    quad.translate( k1 * dx01 + k2 * dx03, k1 * dy01 + k2 * dy03 );
                    painter.drawConvexPolygon( quad );
                }
                else // selected element consists of several coordinates => draw outlines
                {
                    for ( int i = 0; i < selectedArea.size(); i++ )
                    {
                        painter.drawLine( selectedArea.at( i ) );
                    }
                }


                //paint the lines on the plane sides for the selected item
                painter.drawLine( x0 + k1 * dx01,
                                  y0 + k1 * dy01,
                                  x1 + k1 * dx01,
                                  y1 + k1 * dy01 );
                painter.drawLine( x1 + ( dim1 - 1 ) * dx01 + k2 * dx03,
                                  y1 + ( dim1 - 1 ) * dy01 + k2 * dy03,
                                  x2 + ( dim1 - 1 ) * dx01 + k2 * dx03,
                                  y2 + ( dim1 - 1 ) * dy01 + k2 * dy03 );
                painter.drawLine( x2 + ( dim2 - 1 ) * dx03 + k1 * dx01,
                                  y2 + ( dim2 - 1 ) * dy03 + k1 * dy01,
                                  x3 + ( dim2 - 1 ) * dx03 + k1 * dx01,
                                  y3 + ( dim2 - 1 ) * dy03 + k1 * dy01 );
                painter.drawLine( x3 + k2 * dx03,
                                  y3 + k2 * dy03,
                                  x0 + k2 * dx03,
                                  y0 + k2 * dy03 );

                painter.setPen( pen1 );
            }  // if (selected_rects)
        }      // for (k2)
    }          // for (k1)
}
//end of paint()


//determines if the front of the plane is lower then the back part,
//in this case the deepest plane will be painted first,
//otherwise we start with the one on the top, because we see the block from the bottom
bool
Plane::isRising()
{
    double frontZ = points[ 0 ].getZ();
    double frontY = points[ 0 ].getY();

    for ( unsigned i = 1; i < 4; i++ )
    {
        if ( points[ i ].getZ() > frontZ )
        {
            frontZ = points[ i ].getZ();
            frontY = points[ i ].getY();
        }
    }
    return frontY > points[ 4 ].getY();
}

//returns the corner points of a rectangle around the plane
void
Plane::getCornerPoints( double& minX, double& maxX, double& minY, double& maxY )
{
    minX = points[ 0 ].getX();
    maxX = minX;
    minY = points[ 0 ].getY();
    maxY = minY;

    double x, y;
    for ( unsigned int i = 0; i < 5; i++ )
    {
        x = points[ i ].getX();
        y = points[ i ].getY();
        if ( x < minX )
        {
            minX = x;
        }
        else if ( x > maxX )
        {
            maxX = x;
        }
        if ( y < minY )
        {
            minY = y;
        }
        else if ( y > maxY )
        {
            maxY = y;
        }
    }
    assert( maxX >= minX );
    assert( maxY >= minY );
}

//returns the most-top-most-left point of the plane
void
Plane::getTopLeftPoint( double& x, double& y, double& z )
{
    x = points[ 0 ].getX();
    y = points[ 0 ].getY();
    z = points[ 0 ].getZ();

    double x2, y2, z2;
    for ( unsigned int i = 1; i < 4; i++ )
    {
        x2 = points[ i ].getX();
        y2 = points[ i ].getY();
        z2 = points[ i ].getZ();
        if ( x2 < x )
        {
            x = x2;
        }
        if ( y2 < y )
        {
            y = y2;
        }
        if ( z2 < z )
        {
            z = z2;
        }
    }
}

//sets the line type;
//LineType is defined in constants.h;
//lineType stores if the lines between the system items are black, gray,
//white or if no lines should be drawn
void
Plane::setLineType( LineType lineType )
{
    this->lineType = lineType;
}
