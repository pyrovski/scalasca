/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cmath>

#include "Point.h"

using namespace std;


Point::Point()
{
    x = 0;
    y = 0;
    x = 0;
}

Point::Point( double x,
              double y,
              double z )
{
    this->x = x;
    this->y = y;
    this->z = z;
}

//return position
double
Point::getX() const
{
    return x;
}
double
Point::getY() const
{
    return y;
}
double
Point::getZ() const
{
    return z;
}

//set position
void
Point::setX( double x )
{
    this->x = x;
}
void
Point::setY( double y )
{
    this->y = y;
}
void
Point::setZ( double z )
{
    this->z = z;
}

//rotate this point around the X axis
void
Point::xRotate( double angle )
{
    angle = angle / 180 * 3.1415926535;
    double r = sqrt( y * y + z * z );
    if ( r != 0 )
    {
        double alpha = acos( z / r );
        if ( y < 0 )
        {
            alpha = -alpha;
        }
        z = r * cos( alpha + angle );
        y = r * sin( alpha + angle );
    }
}

//rotate this point around the Y axis
void
Point::yRotate( double angle )
{
    angle = angle / 180 * 3.1415926535;
    double r = sqrt( x * x + z * z );
    if ( r != 0 )
    {
        double alpha = acos( x / r );
        if ( z < 0 )
        {
            alpha = -alpha;
        }
        x = r * cos( alpha + angle );
        z = r * sin( alpha + angle );
    }
}

//rotate this point around the Z axis
void
Point::zRotate( double angle )
{
    angle = angle / 180.0 * 3.1415926535;
    double r = sqrt( x * x + y * y );
    if ( r != 0 )
    {
        double alpha = acos( x / r );
        if ( y < 0 )
        {
            alpha = -alpha;
        }
        x = r * cos( alpha + angle );
        y = r * sin( alpha + angle );
    }
}

//scroll this point along the X axis
void
Point::xScroll( double value )
{
    x += value;
}

//scroll this point along the Y axis
void
Point::yScroll( double value )
{
    y += value;
}

//scroll this point along the Z axis
void
Point::zScroll( double value )
{
    z += value;
}

//mirror this point on the different axes
void
Point::xMirror()
{
    x = -x;
}
void
Point::yMirror()
{
    y = -y;
}
void
Point::zMirror()
{
    z = -z;
}


//scale the distance from the argument point (x,y,z)
//to this point by the parameter factor
//
void
Point::scale( double x, double y, double z, double factor )
{
    this->x = x + ( this->x - x ) * factor;
    this->y = y + ( this->y - y ) * factor;
    this->z = z + ( this->z - z ) * factor;
}
