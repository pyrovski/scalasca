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

#ifndef EARL_WINDOW_H
#define EARL_WINDOW_H

/*
 *----------------------------------------------------------------------------
 *
 * classMPIWindow
 *
 * RMA memory window, wich is shared by processes
 * in communicator com.
 *
 *----------------------------------------------------------------------------
 */

namespace earl
{
  class Communicator;

  class MPIWindow
  {
  public:

    MPIWindow(long          id,  
              Communicator* com) :
      id(id), com(com) {}

    long           get_id()    const { return id;  }
    Communicator*  get_com()   const { return com; }
  
  private:
    long          id;
    Communicator* com;
  };
}
#endif


