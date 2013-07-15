/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _POINT_H
#define _POINT_H

//this class offers the representation of 3D-points
//and transformations on them

class Point
{
public:

    Point();
    Point( double x,
           double y,
           double z );

    //rotate this point around the different axes
    void
    xRotate( double angle );
    void
    yRotate( double angle );
    void
    zRotate( double angle );

    //scroll this point along the different axes
    void
    xScroll( double value );
    void
    yScroll( double value );
    void
    zScroll( double value );

    //mirror this point on the different axes
    void
    xMirror();
    void
    yMirror();
    void
    zMirror();

    //scale the distance from the argument point (x,y,z)
    //to this point by the parameter factor
    void
    scale( double x,
           double y,
           double z,
           double factor );

    //return the position
    double
    getX() const;
    double
    getY() const;
    double
    getZ() const;

    //set the position
    void
    setX( double x );
    void
    setY( double y );
    void
    setZ( double z );

private:

    //the position components
    double x, y, z;
};

#endif
