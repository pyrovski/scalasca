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

#include <Controller.h>

/*
 *---------------------------------------------------------------------------
 *
 * class Controller
 *
 *---------------------------------------------------------------------------
 */


/*
 * Determine the controll variable gamm for the nes pass.
 */

void Controller::set_gamma(int pass) {
  if (pass == 1)
    gamma = 0.99999; 
  else if (pass == 2)
    gamma = 0.9999;
  else if (pass == 3)
    gamma = 0.999;
  else
    gamma = 0.9 * gamma;
}
