/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef __CONTROLLER_H
#define __CONTROLLER_H


/*
 *---------------------------------------------------------------------------
 *
 * class Controller
 *
 *---------------------------------------------------------------------------
 */

class Controller
{
 public:
  /* Constructors & destructor */
  Controller(): gamma(0.99999) {}

  ~Controller(){}
  
  double get_gamma () { return gamma; }
  void   set_gamma (int pass);
  
 private:
  /* Data */
  // Control variable. 
  double gamma;
};

#endif   /* !__CONTROLLER_H */
