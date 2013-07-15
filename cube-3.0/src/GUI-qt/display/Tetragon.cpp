/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Tetragon.h"
#include <cassert>
#include <iostream>

Tetragon::Tetragon() : QPolygonF(){}


//check if a point is contained in the tetragon
//
bool Tetragon::containsPoint(const QPointF & p) const{

  assert(count()==4);

  /*

     P0---------------P1
       \              \
        \              \
         \     . (x,y)  \
          \              \
           \              \
            \              \
             \              \
            P3---------------P2

  the point (x,y) is inside the tetragon if it is 
  - "in the angle" at P0 between the vectors P0 to P1 and P0 to P3 and
  - "in the angle" at P2 between the vectors P2 to P1 and P2 to P3

  */

  double x = (double)(p.x());
  double y = (double)(p.y());

  //position P0 
  double x0 = at(0).x();
  double y0 = at(0).y();

  //vector from P0 to P3
  double x1 = at(3).x() - x0;
  double y1 = at(3).y() - y0;

  //vector from P0 to P1
  double x2 = at(1).x() - x0;
  double y2 = at(1).y() - y0;
  x2 = -x2;
  y2 = -y2;

  double det12 = (double)(x1 * y2 - y1 * x2);
  if (det12==0.0) return false;

  double a = ((x * y2 - y * x2) - (x0 * y2 - y0 * x2)) / det12;

  if (a<0.0) {
    return false;
  }
  double b = ((x * y1 - y * x1) - (x0 * y1 - y0 * x1)) / det12;

  if (b<0.0) {
    return false;
  }

  // ok, point is "in the angle" at P0 between the vectors P0 to P1 and P0 to P3
  // now check if point is "in the angle" at P2 between the vectors P2 to P1 and P2 to P3

  x0 = at(2).x();
  y0 = at(2).y();

  x1 = at(1).x() - x0;
  y1 = at(1).y() - y0;

  x2 = at(3).x() - x0;
  y2 = at(3).y() - y0;
  x2 = -x2;
  y2 = -y2;

  det12 = (double)(x1 * y2 - y1 * x2);
  if (det12==0.0) return false;

  a = (double)((x * y2 - y * x2) - (x0 * y2 - y0 * x2)) / det12;

  if (a<0.0) {
    return false;
  }

  b = (double)((x * y1 - y * x1) - (x0 * y1 - y0 * x1)) / det12;

  if (b<0.0) {
    return false;
  }

  //ok, point is also "in the angle" at P2 between the vectors P2 to P1 and P2 to P3
  //and is thus contained in the tetragon
  return true;

}
