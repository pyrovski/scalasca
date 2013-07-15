/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SALSA_H
#define SALSA_H

#include "GlobalDefs.h"
#include "CallbackData.h"

enum funcs{count=1, length, duration, rate};
enum modes{minimum=1, maximum, avg, sum};
enum formats {ascii=1, binary, sion};


class CallbackDataneu:public pearl::CallbackData {
  public:
  pearl::GlobalDefs* defs;
};

#endif
