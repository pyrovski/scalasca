/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _SystemTopologyDrawing_H
#define _SystemTopologyDrawing_H

#include <QtGui>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#include <vector>
#include <math.h>
#include <iostream>
#include <map>

#include "Plane.h"
#include "Constants.h"

#include "Cube.h"
#include "SystemTopologyData.h"
#include "SystemTopologyView.h"
#include "SystemTopologyViewTransform.h"

class QSettings;
class InfoToolTip;

class SystemTopologyDrawing : public SystemTopologyView
{
    Q_OBJECT

public slots:
    /** recreates drawing if values have changed */
    void
    updateDrawing();
    void
    rescaleDrawing();
    void
    changeZoom( double factor );
    void
    setXAngle( int xangle );
    void
    setYAngle( int yangle );
    void
    move( int dx,
          int dy );

public:
    SystemTopologyDrawing( SystemTopologyData*          data,
                           SystemTopologyViewTransform* transform );
    ~SystemTopologyDrawing();
    void
    setSize( const QSize &size );

protected:
    //main painting method
    void
    paintEvent( QPaintEvent* );

    //mouse and key event handling
    void
    mousePressEvent( QMouseEvent* event );
    void
    mouseMoveEvent( QMouseEvent* event );
    void
    mouseReleaseEvent( QMouseEvent* event );
    void
    wheelEvent( QWheelEvent* e );
    void
    keyPressEvent( QKeyEvent* event );
    void
    keyReleaseEvent( QKeyEvent* event );

private:
    // main drawing method
    void
    draw();
    void
    drawOffScreen();
    void
    drawOnScreen();
    QSize
    getDrawingSize();
    void
    paintTopology( QPainter &painter,
                   bool      optimizeScreen = false );

    //some transformation functions
    void
    rotateTo( const QPoint &endPoint );
    void
    moveTo( const QPoint &endPoint );
    void
    setDims( unsigned dim1,
             unsigned dim2,
             unsigned dim3 );

    void
    initScale();

    //initialize the plane
    void
    initPlane();
    void
    scale( int planeDistance );

    // rescale the topology such that the topology fits into the the current visible scrollArea
    void
    rescale( bool resetPlaneDistance = false );

    // resets zoom factor, angles and plane distance
    void
    reset();

    //re-compute the colors
    void
    updateColors();

    //returns the maximum of topology drawing size and widget size
    QSize
    getMinimumPixmapSize();

    //returns length of the top horizontal line of the drawing
    int
    getBaseLineWidth();

    //methods for the topology tool bar
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();

    // ---------------------------------------------------

    //we store only one plane and shift it for paining if several planes are defined
    Plane plane;

    QSize parentSize; // visible part of the Drawing

    //for mouse and key event handling
    double       widgetScaleFactor; // factor to scale from minimum to widget size
    QPoint       lastPoint, firstPoint;
    bool         leftMousePressed;
    bool         rightMousePressed;
    bool         shiftPressed;
    bool         controlPressed;
    bool         toSelect; // newly selected element has to be marked
    int          selectedSystemId;

    QPixmap*     offscreen; // used for double buffering
    InfoToolTip* info;      // info tooltip

    // interactive changes of the view: scaling, rotating, moving
    int                          prevXAngle;
    int                          prevYAngle;

    SystemTopologyViewTransform* transform;
    SystemTopologyData*          data;
};

class InfoToolTip : public QFrame
{
    QString left;
    QString right;
public:
    InfoToolTip();
    void
    paintEvent( QPaintEvent* );
    void
    showInfo( const QPoint &     pos,
              const QStringList &tooltipText );
};

#endif
