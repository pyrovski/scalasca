/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _TETRAGON_H
#define _TETRAGON_H

#include <QPolygonF>

//the QPolygonF class restricted to tetragons and 
//with a rewritten containsPoint function
//(see e.g. http://mathworld.wolfram.com/TriangleInterior.html)

class Tetragon : public QPolygonF {

 public:

  Tetragon();

  //determine if a point is contained in the quad
  bool containsPoint(const QPointF & point) const;

};

#endif
