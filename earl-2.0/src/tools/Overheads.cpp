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

#include "Overheads.h"

#include <cstdio>
#include <iostream>

#include <elg_error.h>

using namespace std;

Overheads::Overheads()
{
  cerr << "Parsing overhead file...";

  // open overhead file
  FILE* file = fopen("ovrhds.out", "r");
  if ( file == NULL )
    elg_error();

  // populate bandwidth table
  size_t buf_size;
  double bandw;
  int result;

  while ( true )
    {
      result = fscanf(file, "MEMORY_BANDW %d %le\n", &buf_size, &bandw);
            
      if (  result == 2 )
	{
	  bandwm[buf_size] = bandw;
	  //elg_cntl_msg("MEMORY_BANDW %d %le", buf_size, bandw);
	}
      else if ( result == EOF )
	{
	  elg_error_msg("Cannot parse overhead file");
	}
      else
	break;
    }
  result = fscanf(file, "EVENT_OVRHD %le\n", &event_ovrhd);

  if ( result != 1 )
    if ( result == EOF )
      elg_error_msg("Cannot parse overhead file");
    else
      elg_error_msg("Cannot parse event overhead");
  
  // close overhead file
  fclose(file);

  if ( bandwm.size() == 0 )
    elg_error_msg("Not enough bandwidth values in overhead file");

  cerr << "      done" << endl;
}
  
double 
Overheads::get_m2mcpy_time(size_t buf_size)
{
  double bandw = 0.0;
  size_t max_buf_size = (++bandwm.end())->first;;

  if ( buf_size >= max_buf_size )
    {
      bandw = bandwm.find(max_buf_size)->second;
    }
  else
    {
      map<size_t, double>::const_iterator it;
      for ( it = bandwm.begin(); it != bandwm.end(); it++ )
	{
	  if ( buf_size < it->first )
	    bandw = it->second;
	}
    }
  return (double)(buf_size) / bandw;
}

