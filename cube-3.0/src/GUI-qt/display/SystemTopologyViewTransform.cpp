/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "SystemTopologyViewTransform.h"

SystemTopologyViewTransform::SystemTopologyViewTransform( SystemTopologyData* data )
{
    this->data          = data;
    absoluteScaleFactor = 1.;
    angleHasChanged     = false;
    distanceChanged     = false;
    planeDistance       = 10;
    xAngle              = 300;
    yAngle              = 30;
}

int
SystemTopologyViewTransform::getXAngle() const
{
    return xAngle;
}
int
SystemTopologyViewTransform::getYAngle() const
{
    return yAngle;
}

/**
   rescales the drawing to fit into the widget and resets scrollbar position to top left
   @param resetValues if true (default), resets angles of topology and plane distance
 */
void
SystemTopologyViewTransform::rescale( bool resetValues )
{
    absoluteScaleFactor = 1.;

    // only reset the angle if desired
    if ( resetValues )
    {
        angleHasChanged = false;
        distanceChanged = false;
        int dims = 0;
        for ( int i = 0; i < 3; i++ )
        {
            if ( data->getDim( i ) > 1 )
            {
                dims++;
            }
        }
        if ( dims > 2 )
        {
            setXAngle( 300 );
            setYAngle( 30 );
        }
        else
        {
            setXAngle( 0 );
            setYAngle( 0 );
        }
    }
    if ( !distanceChanged ) // user has not changed the plane distance
    {
        planeDistance = 0;  // recalculate plane distance
    }
    emit rescaleRequest();
}

//if the x/y angle has been changed internally, then we should
//additionally emit the corresponding signal;
//if it has been changed externally, then the signal is emitted
//by the x/y spin

void
SystemTopologyViewTransform::setXAngle( int angle )
{
    /* user controls (spin box) also sends events, if value was changed by program.
       => don't emit signal, if value hasn't changed
     */
    if ( xAngle != angle )
    {
        while ( angle >= 360 )
        {
            angle -= 360;
        }
        while ( angle < 0 )
        {
            angle += 360;
        }
        xAngle          = angle;
        angleHasChanged = true;
        emit xAngleChanged( xAngle );
    }
}

void
SystemTopologyViewTransform::setYAngle( int angle )
{
    if ( yAngle != angle )
    {
        while ( angle >= 360 )
        {
            angle -= 360;
        }
        while ( angle < 0 )
        {
            angle += 360;
        }
        yAngle          = angle;
        angleHasChanged = true;
        emit yAngleChanged( yAngle );
    }
}

void
SystemTopologyViewTransform::increasePlaneDistance()
{
    planeDistance  += 1;
    distanceChanged = true;
    emit viewChanged(); //todo other signal
}

void
SystemTopologyViewTransform::decreasePlaneDistance()
{
    if ( planeDistance >= 2 )
    {
        planeDistance  -= 1;
        distanceChanged = true;
        emit viewChanged();
    }
}

void
SystemTopologyViewTransform::zoom( double factor )
{
    relativeScaleFactor  = factor;
    absoluteScaleFactor *= factor;
    planeDistance       *= factor;
}

void
SystemTopologyViewTransform::zoomIn()
{
    emit zoomChanged( 1.1 ); // notify view to check if zooming in is valid
}

void
SystemTopologyViewTransform::zoomOut()
{
    emit zoomChanged( 0.9 );
}

void
SystemTopologyViewTransform::moveDown()
{
    emit positionChanged( 0, 5 );
}

void
SystemTopologyViewTransform::moveUp()
{
    emit positionChanged( 0, -5 );
}

void
SystemTopologyViewTransform::moveRight()
{
    emit positionChanged( 5, 0 );
}

void
SystemTopologyViewTransform::moveLeft()
{
    emit positionChanged( -5, 0 );
}

void
SystemTopologyViewTransform::saveSettings( QSettings& settings, int topologyId )
{
    QString groupName( "systemTopologyViewTransform" );
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    settings.setValue( "planeDistance", planeDistance );
    settings.setValue( "xAngle", xAngle );
    settings.setValue( "yAngle", yAngle );

    settings.endGroup();
}

bool
SystemTopologyViewTransform::loadSettings( QSettings& settings, int topologyId )
{
    QString groupName( "systemTopologyViewTransform" );
    groupName.append( QString::number( topologyId ) );

    settings.beginGroup( groupName );

    planeDistance = settings.value( "planeDistance", 1 ).toInt();
    xAngle        = settings.value( "xAngle", 300 ).toInt();
    yAngle        = settings.value( "yAngle", 30 ).toInt();

    settings.endGroup();

    emit xAngleChanged( xAngle );
    emit yAngleChanged( yAngle );

    return true;
}
