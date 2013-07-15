/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef   _SYSTEMTOPOLOGYVIEW_H
#define   _SYSTEMTOPOLOGYVIEW_H

#include <QWidget>
#include "SystemTopologyView.h"

class SystemTopologyView : public QWidget
{
    Q_OBJECT

public:
    virtual void
    updateDrawing() = 0;
    virtual void
    rescaleDrawing() = 0;
    virtual void
    changeZoom( double zoomFactor ) = 0;
    virtual void
    setXAngle( int xangle ) = 0;
    virtual void
    setYAngle( int yangle ) = 0;
    virtual void
    setSize( const QSize &visibleRegion ) = 0;
    virtual void
    move( int dx,
          int dy ) = 0;

signals:
    // selectItem is emitted, if an element is selected with left mouse click
    void
    selectItem( int id );
};

#endif
