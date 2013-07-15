/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef TOPOLOGYVIEWPARAMETERS_H
#define TOPOLOGYVIEWPARAMETERS_H

#include <QtGui>
#include "SystemTopologyData.h"

/**
 * @brief contains data and methods which transform the view, e.g. rotation, scaling
 */
class SystemTopologyViewTransform : public QObject
{
    Q_OBJECT
public slots:
    void
    increasePlaneDistance();
    void
    decreasePlaneDistance();
    void
    rescale( bool resetValues = true );
    void
    setXAngle( int angle );
    void
    setYAngle( int angle );
    void
    zoomIn();
    void
    zoomOut();
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();

signals:
    void
    rescaleRequest();
    void
    viewChanged();
    void
    zoomChanged( double factor );
    void
    xAngleChanged( int );
    void
    yAngleChanged( int );
    void
    positionChanged( int,
                     int );

public:
    SystemTopologyViewTransform( SystemTopologyData* data );
    bool
    distanceHasChanged() const
    {
        return distanceChanged;
    }
    QSize
    getWidgetSize()
    {
        return size;
    }                                    // returns the widget size
    int
    getPlaneDistance() const
    {
        // planeDistance is used as integer for drawing, but saved as double to
        // keep same value after zooming in and out
        return ( int )planeDistance;
    }
    int
    getXAngle() const;
    int
    getYAngle() const;

    double
    getAbsoluteScaleFactor()
    {
        return absoluteScaleFactor;
    }
    double
    getRelativeScaleFactor()
    {
        return relativeScaleFactor;
    }
    void
    setPlaneDistance( int d )
    {
        planeDistance = d;
    }
    void
    zoom( double factor );
    void
    saveSettings( QSettings &settings,
                  int        topologyId );
    bool
    loadSettings( QSettings &settings,
                  int        topologyId );

private:
    double              planeDistance; // distance between planes above each other
    int                 xAngle;        // current angle
    int                 yAngle;
    bool                distanceChanged;
    bool                angleHasChanged;
    double              absoluteScaleFactor; // scale=1.0 = fill the visible area
    double              relativeScaleFactor;
    QSize               size;                // size of the visible part of the drawing

    SystemTopologyData* data;
};


#endif // TOPOLOGYVIEWPARAMETERS_H
