/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef PERTRB_OVERHEADS_H
#define PERTRB_OVERHEADS_H


#include <map>

class Overheads
{
public:

  Overheads();

  double get_m2mcpy_time(size_t buf_size); 
  double get_event_ovrhd() { return event_ovrhd; } 

private: 
  
  std::map<size_t, double> bandwm; 
  double event_ovrhd;
};

#endif
