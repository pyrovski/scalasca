/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "TreeWidget.h"
#include "Tetragon.h"
#include "SystemTopologyDrawing.h"
#include "SystemTopologyViewTransform.h"
#include "PrecisionWidget.h"

#include "Cartesian.h"
#include "Sysres.h"
#include "Thread.h"
#include "Process.h"
#include "Node.h"

#include <cassert>
#include <qdebug.h>

/*************** constructor / destructor *************************/

SystemTopologyDrawing::SystemTopologyDrawing( SystemTopologyData*          dat,
                                              SystemTopologyViewTransform* transform )
{
    this->data       = dat;
    this->transform  = transform;
    this->prevXAngle = transform->getXAngle();
    this->prevYAngle = transform->getYAngle();
    this->info       = new InfoToolTip();

    offscreen         = NULL;
    leftMousePressed  = false;
    rightMousePressed = false;
    shiftPressed      = false;
    controlPressed    = false;
    toSelect          = false;

    //to handle keyboard input
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( false );

    setMinimumSize( 1, 1 ); // ensure a minimal size of (1,1), if zero no paintEvent is emitted
}


SystemTopologyDrawing::~SystemTopologyDrawing()
{
    delete offscreen;
}

/** This method is called, if the window has been resized.
 *  If the drawing previously has fit into the window, it is rescaled to the new window size.
 */
void
SystemTopologyDrawing::setSize( const QSize &size )
{
    int   eps      = 50;
    QSize drawSize = getDrawingSize();
    bool  doesFit  = ( ( ( drawSize.width() - parentSize.width() ) < eps ) &&
                       ( ( drawSize.height() - parentSize.height() ) < eps ) );
    bool  init = ( parentSize.width() <= 0 );
    parentSize = size;

    if ( doesFit || init )
    {
        transform->rescale( false );
    }

    setMinimumSize( getMinimumPixmapSize() ); // for the scrollbars
    setMaximumSize( getMinimumPixmapSize() );
}

void
SystemTopologyDrawing::updateDrawing()
{
    plane.setLineType( data->getLineType() );
    //qDebug()  << "updateDrawing " <<transform->getScale() << QTime::currentTime();

    initPlane();
    plane.scale( widgetScaleFactor * transform->getAbsoluteScaleFactor() );

    draw();
}

void
SystemTopologyDrawing::rescaleDrawing()
{
    rescale();
    draw();
}

/******************** angle settings ********************/

void
SystemTopologyDrawing::setXAngle( int )
{
    updateDrawing();
}

void
SystemTopologyDrawing::setYAngle( int )
{
    updateDrawing();
}

/********************** initialization / clean up ********************************/

/**
   initializes the plane
   sets its size to its minimum size (min pixels for each element)
   sets the angle given by data
 */
void
SystemTopologyDrawing::initPlane()
{
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }

    const int min = 2;
    double    x   = min * ( double )data->getDim( XX );

    //get height according to width
    double y = x / ( double )data->getDim( XX ) * ( double )data->getDim( YY );
    //minimal height is 3 pixels per square
    if ( y < min * ( double )data->getDim( YY ) )
    {
        y = min * ( double )data->getDim( YY );
        x = y / ( double )data->getDim( YY ) * ( double )data->getDim( XX );
    }

    //get the middle
    x = x / 2;
    y = y / 2;

    //the four plane corners are the points 0 to 3
    plane.setPoint( 0, Point( -x, -y, 0 ) );
    plane.setPoint( 1, Point( x, -y, 0 ) );
    plane.setPoint( 2, Point( x, y, 0 ) );
    plane.setPoint( 3, Point( -x, y, 0 ) );
    //plane point 4 is the middle, needed for certain operations like y
    plane.setPoint( 4, Point( 0, 0, 0 ) );

    plane.xRotate( transform->getXAngle() );
    plane.yRotate( transform->getYAngle() );
}

/************************* coloring  ***********************/

void
SystemTopologyDrawing::updateColors()
{
    draw();
}


/*******************************************/


void
SystemTopologyDrawing::initScale() // todo ???
{                                  //if no topology loaded, return
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }

    //scroll the plane such that it is in the non-negative coordinate area
    double x, y, z;
    plane.getTopLeftPoint( x, y, z );
    if ( x < 0.0 )
    {
        plane.xScroll( -x );
    }
    if ( y < 0.0 )
    {
        plane.yScroll( -y );
    }

    //make sure that the cube fits into the widget
    setMinimumSize( getMinimumPixmapSize() );
    setMaximumSize( getMinimumPixmapSize() );
}

/**
 * @return the size of topology drawing
 */
QSize
SystemTopologyDrawing::getDrawingSize()
{
    int    pad = 5;
    double minX, maxX, minY, maxY;
    plane.getCornerPoints( minX, maxX, minY, maxY );
    int    width  = ( int )maxX + pad;
    int    height = ( int )maxY + pad; // one plane
    height += ( int )( data->getDim( ZZ ) - 1 ) * transform->getPlaneDistance();
    return QSize( width, height );
}

/**
 * @return the maximum of topology drawing size and widget size
 */
QSize
SystemTopologyDrawing::getMinimumPixmapSize()
{
    QSize ret = getDrawingSize();

    // ensure that the drawing size is not smaller than the widget size
    int w = parentSize.width();
    int h = parentSize.height();
    if ( ret.width() < w )
    {
        ret.setWidth( w );
    }
    if ( ret.height() < h )
    {
        ret.setHeight( h );
    }

    return ret;
}

/**
 * @return size of the complete topology drawing
 */
int
SystemTopologyDrawing::getBaseLineWidth()
{
    const Point* point0 = plane.getPoint( 0 );
    const Point* point1 = plane.getPoint( 1 );
    int          width  =  ( ( int )( ( point1->getX() - point0->getX() ) / ( double )data->getDim( XX ) ) ) * data->getDim( YY );

    return width;
}

/**
   scales the plane to fit into the widget
   @param planeDistance
 */
void
SystemTopologyDrawing::scale( int planeDistance )
{
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }

    initPlane(); // scale plane to minimum size

    double minX, maxX, minY, maxY;
    plane.getCornerPoints( minX, maxX, minY, maxY );

    assert( minX <= maxX );
    assert( minY <= maxY );
    assert( maxX - minX > 0 );
    assert( maxY - minY > 0 );

    //current total width and height
    double sumW = maxX - minX;
    double sumH = maxY - minY; // height of topmost plane

    /* if planeDistance is set to 1 (minimum), the available space will be used */
    double planeH = ( data->getDim( ZZ ) - 1 ) * planeDistance; // height to display n planes

    double xScaleFactor = ( parentSize.width() - 2 * TOPOLOGY_MARGIN ) / sumW;
    double yScaleFactor = ( parentSize.height() - planeH - 2 * TOPOLOGY_MARGIN ) / sumH;
    widgetScaleFactor = ( xScaleFactor < yScaleFactor ) ? xScaleFactor : yScaleFactor;
    /* scale factor of 1 shows the plane with its minimal size */
    if ( widgetScaleFactor < 1.0 )
    {
        widgetScaleFactor = 1.0;
    }

    plane.scale( widgetScaleFactor );
}

/**
   scales the topology to fit into the window
   @param resetDistance recalculate plane distance, if true
 */
void
SystemTopologyDrawing::rescale( bool resetPlaneDistance )
{
    if ( data->getDim( XX ) == 0 )
    {
        return;                     // invalid data
    }
    // reset the plane distance if desired or not yet changed
    bool recalculatePlaneDistance = ( resetPlaneDistance || !transform->distanceHasChanged() );

    int  planeDistance = recalculatePlaneDistance ? 1 : transform->getPlaneDistance();
    scale( planeDistance );

    if ( data->getDim( ZZ ) == 1 )
    {
        planeDistance = 1; // only one plane, no need to calculate distance
    }
    else if ( recalculatePlaneDistance )
    {
        double minX, maxX, minY, maxY;
        plane.getCornerPoints( minX, maxX, minY, maxY );

        double distance = ( ( double )( parentSize.height() ) - ( maxY - minY ) - 2 * TOPOLOGY_MARGIN )
                          / ( double )( data->getDim( ZZ ) - 1 );
        if ( distance <= 1.0 )
        {
            transform->setPlaneDistance( 1 );
        }
        else
        {
            transform->setPlaneDistance( ( int )distance );
        }
    }
}

/**
   If no mouse button is pressed, the image data hasn't changed and the image
   can be copied from offscreen image
   If a mouse button is pressed, the image is painted directly on screen to avoid
   unnecessary copy steps
 */
void
SystemTopologyDrawing::paintEvent( QPaintEvent* )
{
    //  qDebug() << "==== paintEv" << QTime::currentTime();
    QPainter painter( this );
    painter.fillRect( rect(), this->palette().brush( QPalette::Window ) );

    // mouse action cannot be handled with offscreen image
    if ( rightMousePressed || leftMousePressed || toSelect )
    {
        drawOnScreen();
    }
    else                                                 // use offscreen image
    {
        bool selectionChanged = data->updateSelection(); // todo optimize
        if ( offscreen == 0 || selectionChanged )
        {
            drawOffScreen();
        }
        painter.drawPixmap( 0, 0, *offscreen );
    }
}

/**
   This method is called, if the data to draw has changed.
   If no mouse button is pressed, the offscreen image will be drawn.
   If mouse is pressed, drawing is done directly on screen using the
   paintEvent method.
 */
void
SystemTopologyDrawing::draw()
{
    // todo notify : void TreeWidgetItem::setSelected(bool selected, bool updateTree)
    // call updateSelection only if a selection has changed
    data->updateSelection(); // todo optimize

    if ( offscreen &&
         ( ( offscreen->width() < parentSize.width() ) ||
           ( offscreen->height() < parentSize.height() ) ) )
    {
        delete offscreen;
        offscreen = 0;
    }

    if ( offscreen == 0 ) // initial call
    {
        drawOffScreen();  // do not call repaint() afterwards, initial draw may have be called from paintEvent()
    }
    else
    {
        if ( !rightMousePressed && !leftMousePressed )
        {
            drawOffScreen();
        }
        repaint();
    }
    //qDebug() << "hier SystemTopologyDrawing::draw() size" << size();
}

void
SystemTopologyDrawing::drawOnScreen()
{
    //if no topology loaded, return
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, data->getAntialiasing() );

    paintTopology( painter, true );
    //qDebug() << "paint on screen " << size() << QTime::currentTime();
}

/**
   draws current topology to offscreen pixmap
 */
void
SystemTopologyDrawing::drawOffScreen()
{
    //if no topology loaded, return
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }
    if ( !isVisible() )
    {
        return;               // avoid unnecessary painting
    }
    // create offscreen image
    const int diff = 400; // resize Pixmap, if more than diff pixel are unused

    QSize     newSize = getMinimumPixmapSize();
    if ( ( offscreen == 0 ) )
    {
        offscreen = new QPixmap( newSize );
    }
    else if ( ( ( offscreen->width() < newSize.width() ) || ( offscreen->height() < newSize.height() ) )
              || ( ( offscreen->width()  - newSize.width() )  > diff )
              || ( ( offscreen->height() - newSize.height() ) > diff ) )
    {
        delete offscreen;
        offscreen = new QPixmap( newSize );
    }

    offscreen->fill( this, 0, 0 );
    QPainter painter( offscreen );
    painter.setRenderHint( QPainter::Antialiasing, data->getAntialiasing() );

    paintTopology( painter );
}

void
SystemTopologyDrawing::paintTopology( QPainter &painter, bool optimizeScreen )
{
    initScale();

    int dimIndex[ 3 ];
    dimIndex[ 0 ] = -1;
    dimIndex[ 1 ] = -1;
    dimIndex[ 2 ] = -1;

    plane.setFoldingSeparator( data->getFoldingSeparators() );
    //if the right mouse is pressed, then we must find the topology item that is selected;
    //the position of the selected item will be stored in dimIndex;
    //similarly, if the left mouse was pressed and released at the same position, we
    //will select the corresponding tree item, and thus need its id

    int selectedPlane = -1;
    if ( rightMousePressed || toSelect )
    {
        // get selected plane index
        Tetragon polygon;
        polygon.push_back( QPointF( plane.getPoint( 0 )->getX(), plane.getPoint( 0 )->getY() ) );
        polygon.push_back( QPointF( plane.getPoint( 1 )->getX(), plane.getPoint( 1 )->getY() ) );
        polygon.push_back( QPointF( plane.getPoint( 2 )->getX(), plane.getPoint( 2 )->getY() ) );
        polygon.push_back( QPointF( plane.getPoint( 3 )->getX(), plane.getPoint( 3 )->getY() ) );

        if ( plane.isRising() ) // check from top to bottom, if user has clicked inside plane
        {
            for ( selectedPlane = 0; selectedPlane < ( int )data->getDim( ZZ ); selectedPlane++ )
            {
//painter.setPen(QPen(Qt::red, 7, Qt::SolidLine)); painter.drawPolygon(polygon);
                if ( polygon.containsPoint( lastPoint ) )
                {
                    break;
                }
                polygon.translate( 0, transform->getPlaneDistance() );
            }
        }
        else // check from bottom to top
        {
            polygon.translate( 0, ( ( int )data->getDim( ZZ ) - 1 ) * transform->getPlaneDistance() );
            for ( selectedPlane = data->getDim( ZZ ) - 1; selectedPlane >= 0; selectedPlane-- )
            {
//painter.setPen(QPen(Qt::red, 7, Qt::SolidLine)); painter.drawPolygon(polygon);
                if ( polygon.containsPoint( lastPoint ) )
                {
                    break;
                }
                polygon.translate( 0, -transform->getPlaneDistance() );
            }
        }
    }
    dimIndex[ ZZ ] = selectedPlane;

    //if the planes are rising, then we paint the lowest plane first,
    //otherwise we start with the plane on the top
    int dummy1 = -1;
    int dummy2 = -1;

    if ( plane.isRising() )
    {
        // since the plane is rising, we paint the lowest plane first
        plane.yScroll( ( double )( data->getDim( ZZ ) - 1 ) * transform->getPlaneDistance() );

        //compute which items on planes are visible if the plane is partly
        //covered by another plane
        plane.computeVisibles( data, -transform->getPlaneDistance() );

        //paint all planes
        for ( int i = data->getDim( ZZ ) - 1; i >= 0; i-- )
        {
            bool toPaint = true;
            if ( optimizeScreen )
            {
                //this is the region of this widget which is not covered by other
                //widgets; only planes in this widget must be painted
                QRect rect = visibleRegion().boundingRect();
                //don't paint if whole plane is below the visible rectangle
                if ( ( int )plane.getPoint( 0 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 1 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 2 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 3 )->getY() > rect.y() + rect.height() )
                {
                    toPaint = false;
                }
                //also don't paint if plane is above the visible rectangle
                else if ( ( int )plane.getPoint( 0 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 1 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 2 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 3 )->getY() < rect.y() )
                {
                    toPaint = false;
                }
            }

            // plane.paint method also calculated the index of the selected item
            if ( toPaint )
            {
                plane.paint( data,
                             i,
                             painter,
                             i > 0,
                             ( i == selectedPlane && ( rightMousePressed || toSelect ) ? &lastPoint : NULL ),
                             ( i == selectedPlane ? dimIndex[ XX ] : dummy1 ),
                             ( i == selectedPlane ? dimIndex[ YY ] : dummy2 ) );
            }
            //scroll the plane to represent the next plane above the
            //currently painted one
            if ( i > 0 )
            {
                plane.yScroll( -transform->getPlaneDistance() );
            }
        }
    }
    else
    {
        //compute which items on planes are visible if the plane is partly
        //covered by another plane
        plane.computeVisibles( data, transform->getPlaneDistance() );

        //paint all planes; since the plane is not rising, we paint the
        //plane on the top first
        for ( unsigned i = 0; i < data->getDim( ZZ ); i++ )
        {
            bool toPaint = true;
            if ( optimizeScreen )
            {
                //this is the region of this widget which is not covered by other
                //widgets; only planes in this widget must be painted
                QRect rect = visibleRegion().boundingRect();
                //don't paint if whole plane is below the visible rectangle
                if ( ( int )plane.getPoint( 0 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 1 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 2 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 3 )->getY() > rect.y() + rect.height() )
                {
                    toPaint = false;
                }
                //also don't paint if plane is above the visible rectangle
                else if ( ( int )plane.getPoint( 0 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 1 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 2 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 3 )->getY() < rect.y() )
                {
                    toPaint = false;
                }
            }
            if ( toPaint )
            {
                plane.paint( data,
                             i,
                             painter,
                             i < data->getDim( ZZ ) - 1,
                             ( ( int )i == selectedPlane && ( rightMousePressed || toSelect ) ? &lastPoint : NULL ),
                             ( ( int )i == selectedPlane ? dimIndex[ XX ] : dummy1 ),
                             ( ( int )i == selectedPlane ? dimIndex[ YY ] : dummy2 ) );
            }
            if ( i < data->getDim( ZZ ) - 1 )
            {
                plane.yScroll( transform->getPlaneDistance() );
            }
        }
        //scroll the plane to represent the next plane below the
        //currently painted one
        plane.yScroll( -( double )( data->getDim( ZZ ) - 1 ) * transform->getPlaneDistance() );
    }

    QPen pen( Qt::black );
    pen.setWidth( 0 );
    painter.setPen( pen );

    //if the left mouse button was pressed and released without moving
    //then we select the corresponding item
    if ( toSelect && dimIndex[ 0 ] != -1 && dimIndex[ 1 ] != -1 && dimIndex[ 2 ] != -1 )
    {
        int systemId = data->getSystemId( dimIndex[ XX ], dimIndex[ YY ], dimIndex[ ZZ ] );
        if ( systemId >= 0 )
        {
            selectedSystemId = systemId;
        }
        else
        {
            selectedSystemId = -1;
        }
    }
    else
    {
        selectedSystemId = -1;
    }

    if ( rightMousePressed )   // show Tooltip
    {
        int    x   = dimIndex[ XX ];
        int    y   = dimIndex[ YY ];
        int    z   = dimIndex[ ZZ ];
        QPoint pos = this->mapToGlobal( lastPoint );
        pos.rx() += 5;
        pos.ry() += 5;
        info->showInfo( pos, data->getTooltipText( x, y, z ) );
    }

    if ( data->hasInvalidDimensions() )
    {
        painter.setOpacity( 0.7 );
        painter.fillRect( this->rect(), Qt::gray );
    }
}

/**************** mouse and key event handling ******************/

//handle mouse press events
//
void
SystemTopologyDrawing::mousePressEvent( QMouseEvent* event )
{
    Qt::KeyboardModifiers mods = event->modifiers();
    //check if shift is pressed
    if ( mods.testFlag( Qt::ShiftModifier ) )
    {
        shiftPressed = true;
    }
    else
    {
        shiftPressed = false;
    }
    //check if control is pressed
    if ( mods.testFlag( Qt::ControlModifier ) )
    {
        controlPressed = true;
    }
    else
    {
        controlPressed = false;
    }

    //get the position of the click
    lastPoint = event->pos();
    //for left-mouse-pull remember the first point to compute the
    //difference later for mouse movement
    if ( !leftMousePressed )
    {
        firstPoint = lastPoint;
    }
    //left button is handled at mouse release or mouse movement
    if ( event->button() == Qt::LeftButton )
    {
        leftMousePressed = true;
        event->accept();
    }
    else if ( event->button() == Qt::RightButton )
    {
        //right button is for context info (painted by  draw())
        rightMousePressed = true;
        draw();
        event->accept();
    }
    else
    {
        event->ignore();
    }

    data->updateSelection();
}

//handle mouse release events
//
void
SystemTopologyDrawing::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        leftMousePressed = false;
        //if the left mouse button is pressed and released at the same point
        //then we select the corresponding tree item
        if ( firstPoint == event->pos() )
        {
            toSelect = true;
            draw();
            toSelect = false;
            if ( selectedSystemId >= 0 )
            {
                emit selectItem( selectedSystemId );
            }
        }
        event->accept();
    }
    else if ( event->button() == Qt::RightButton )
    {
        //remove the info box for the previously right-clicked item
        rightMousePressed = false;
        event->accept();
        info->hide();
    }
    else
    {
        event->ignore();
    }

    data->updateSelection();
    draw();
}

//handle mouse move events
//
void
SystemTopologyDrawing::mouseMoveEvent( QMouseEvent* event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        //if mouse is left-clicked and moved while shift is pressed, then
        //move the topology
        if ( shiftPressed )
        {
            moveTo( event->pos() );
        }
        //if mouse is left-clicked and moved while control is pressed,
        //then increase the plane distance
        else if ( controlPressed )
        {
            int planeDistance = transform->getPlaneDistance();
            planeDistance += ( int )event->pos().y() - lastPoint.y();
            if ( planeDistance <= 0.0 )
            {
                planeDistance = 1;
            }
            lastPoint = event->pos();
            transform->setPlaneDistance( planeDistance );
            draw();
        }
        //if mouse is left-clicked and moved, rotate the topology
        else
        {
            rotateTo( event->pos() );
        }
        event->accept();
    }
    else if ( event->buttons() & Qt::RightButton )
    {
        //if mouse is right-clicked and moved, then change the item for
        //which the info box is displayed
        lastPoint = event->pos();
        draw();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//handle mouse wheel events
//
void
SystemTopologyDrawing::wheelEvent( QWheelEvent* e )
{
    //the mouse wheel can be used for zooming
    if ( e->delta() > 0 )
    {
        transform->zoomIn();
    }
    else
    {
        transform->zoomOut();
    }
    e->accept();
}

void
SystemTopologyDrawing::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Shift:
            if ( !controlPressed )
            {
                shiftPressed = true;
            }
            event->accept();
            break;
        case Qt::Key_Control:
            if ( !shiftPressed )
            {
                controlPressed = true;
            }
            event->accept();
            break;
        default:
        { event->ignore();
        }
    }
}

void
SystemTopologyDrawing::keyReleaseEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Shift:
            shiftPressed = false;
            event->accept();
            break;
        case Qt::Key_Control:
            controlPressed = false;
            event->accept();
            break;
        default:
        { event->ignore();
        }
    }
}


/****************** basic plane transformations ************************/

void
SystemTopologyDrawing::rotateTo( const QPoint &endPoint )
{
    int dx = endPoint.x() - lastPoint.x();
    int dy = endPoint.y() - lastPoint.y();
    lastPoint = endPoint;
    int diffX = dx % 10;
    int diffY = dy % 10;

    if ( transform->getYAngle() <= 90 || transform->getYAngle() >= 270 )
    {
        diffY = -diffY;
    }

    int xAngle = transform->getXAngle() - diffY;
    int yAngle = transform->getYAngle() - diffX;

    if ( xAngle != prevXAngle )
    {
        transform->setXAngle( xAngle );
    }
    if ( yAngle != prevYAngle )
    {
        transform->setYAngle( yAngle );
    }
}

void
SystemTopologyDrawing::moveTo( const QPoint &endPoint )
{
    int dx = endPoint.x() - lastPoint.x();
    int dy = endPoint.y() - lastPoint.y();
    plane.xScroll( dx );
    plane.yScroll( dy );
    lastPoint = endPoint;

    draw();
}

void
SystemTopologyDrawing::move( int dx, int dy )
{
    plane.xScroll( dx );
    plane.yScroll( dy );

    draw();
}

void
SystemTopologyDrawing::changeZoom( double factor )
{
    // check if minimum zoom level has been reached
    int oldWidth           = getBaseLineWidth();
    int minPixelPerElement = data->getLineType() == NO_LINES ? 1 : 2;
    int minWidth           = data->getDim( XX ) * minPixelPerElement;
    //qDebug() << "changeZoom:" << oldWidth << minWidth << transform->getAbsoluteScaleFactor();
    if ( ( factor < 1 ) && ( oldWidth <= minWidth ) )
    {
        return;                                         // minimum size reached
    }
    // if BaseLineWidth isn't reduced to number of elements in x direction,
    // ensure that recursion is aborted
    bool factorTooSmall = transform->getAbsoluteScaleFactor() < ( 1. * data->getDim( XX ) / parentSize.width() );
    if ( factorTooSmall && factor < 1 )
    {
        return;
    }

    // set data
    transform->zoom( factor );

    // scale and draw topology
    double x, y, z;
    plane.getTopLeftPoint( x, y, z );
    plane.scale( x, y, z, transform->getRelativeScaleFactor() );
    int newWidth = getBaseLineWidth();


    // if result hasn't changed, try to zoom with the same factor again
    bool changed = oldWidth != newWidth;
    if ( !changed && ( newWidth >= minWidth ) )
    {
        changeZoom( factor );
    }
    else
    {
        draw();
    }
}

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

InfoToolTip::InfoToolTip()
{
    setWindowFlags( Qt::ToolTip );
}

void
InfoToolTip::showInfo( const QPoint &pos, const QStringList &tooltipText )
{
    if ( tooltipText.size() == 2 )
    {
        left  = tooltipText.at( 0 );
        right = tooltipText.at( 1 );
        move( pos );
        repaint();
        show();
    }
    else
    {
        hide();
    }
}

void
InfoToolTip::paintEvent( QPaintEvent* )
{
    if ( left.isEmpty() )
    {
        return;
    }

    QPainter painter( this );
    painter.setBrush( Qt::white );
    painter.setPen( Qt::black );

    //get height and width of the text blocks
    QRect rect1 = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, left );
    QRect rect2 = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, right );

    QRect rect( 0, 0, rect1.width() + rect2.width() + 11,
                ( rect1.height() > rect2.height() ? rect1.height() + 6 : rect2.height() + 6 ) );

    setMinimumSize( rect.size() );
    setMaximumSize( rect.size() );

    rect.adjust( 0, 0, -1, -1 );
    painter.drawRect( rect );
    rect1.moveTo( rect.x() + 3, rect.y() + 3 );
    painter.drawText( rect1, Qt::AlignLeft, left );
    rect2.moveTo( rect1.x() + rect1.width() + 5, rect1.y() );
    painter.drawText( rect2, Qt::AlignLeft, right );
}
