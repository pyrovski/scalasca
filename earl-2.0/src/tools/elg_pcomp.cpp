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

/****************************************************************************
**  Perturbation Compensation Filter                                       **
**                                                                         **
****************************************************************************/


#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include <earl.h>
#include <elg_error.h>

#include "Compensator.h"

using namespace std;

/*
 *---------------------------------------------------------------------------------
 * Main
 *---------------------------------------------------------------------------------
 */

int 
main(int argc, char** argv)
{
  char*  infile_name;
  char*  outfile_name;
  approx_mode amode = LOWER_BOUND;

  /* ckeck # of arguments */
  if ( argc < 3 || argc > 4 )
    elg_error_msg("Wrong # of arguments\nUsage: elg_pcomp [--upper | --lower] <infile> <outfile>" );

  vector<char*> file_namev;
  char* cur;
  for ( int i = 1; i < argc; i++ ) 
    {
      cur = argv[i];
      if ( string(cur) == "--upper" )
	amode = UPPER_BOUND;
      else if ( string(cur) == "--lower" )
	amode = LOWER_BOUND;
      else
	file_namev.push_back(cur);
    }

  if ( file_namev.size() < 2 )
    elg_error_msg("Wrong arguments\nUsage: elg_pcomp [--upper | --lower] <infile> <outfile>" );

  infile_name  = file_namev[0];
  outfile_name = file_namev[1];

  Compensator* comp;
  try 
    {
      comp = new Compensator(infile_name);
      comp->compensate_trace(outfile_name, amode);
    }
  catch(earl::Error e)
    {
      cerr << e.get_msg() << endl;
      exit(1);
    }
  delete comp;
}
















