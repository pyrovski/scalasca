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
**  Measurement of platform-specific overheads                             **
**                                                                         **
****************************************************************************/

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sys/time.h>
#include <iostream>
#include <vector>

#include <elg_error.h>
#include <elg_gen.h>
#include <elg_pform.h>

using namespace std;

const size_t min_buf_size = 64;
const size_t max_buf_size = 2097152;

double get_timer_ovrhd()
{
  static double ovrhd = 0.0;

  if ( fabs(ovrhd) != 0.0 )
    return ovrhd;

  const int n = 200;
  double start, end;

  start = elg_pform_wtime();
  for ( int i = 0; i < n; i++ )
    end = elg_pform_wtime();

  ovrhd = (end - start) / n;
  return ovrhd;
}

double get_bandw(size_t buf_size)
{
  int n = 10;
  double start, end;

  vector<char*> src_bufv;
  vector<char*> dest_bufv;
  
  for ( int i = 0; i < n; i++ )
    {
      char* src  = (char*) malloc(buf_size * sizeof(char));
      if ( src == NULL )
	elg_error();
      src_bufv.push_back(src);
      
      char* dest = (char*) malloc(buf_size * sizeof(char));
      if ( dest == NULL )
	elg_error();
      dest_bufv.push_back(dest);
    }

  start = elg_pform_wtime();
  for ( int i = 0; i < n; i++ )
    memcpy(dest_bufv[i], src_bufv[i], buf_size);
  end = elg_pform_wtime();

  for ( int i = 0; i < n; i++ )
    {
      free(src_bufv[i]);
      free(dest_bufv[i]);
    }
  double duration = ( end - start - get_timer_ovrhd() ) / n;

  return (double)(buf_size) / duration;
}

double get_event_ovrhd()
{
  double start, end;
  char* tmp_name = tempnam("./", NULL); 
  const int nevents = 1000;

  ElgGen* gen = ElgGen_open(tmp_name, ELG_NO_ID, 100000);

  start = elg_pform_wtime();
  for ( int i = 0; i < nevents; i++ )
    {
      ElgGen_write_ENTER(gen, 0, 0, NULL);
      ElgGen_write_EXIT(gen, 0, NULL);
    }
  end = elg_pform_wtime();

  ElgGen_close(gen);
  ElgGen_delete(gen);
  return (end - start - get_timer_ovrhd() ) / ( 2 * nevents );
}

int 
main(int argc, char** argv)
{
  size_t buf_size, step; 

  // initialize platform timer
  elg_pform_init();

  FILE* file = fopen("ovrhds.out", "w");
  if ( file == NULL )
    elg_error();
  
  // generate actual bandwidth table
  double memory_bandw;
  buf_size = min_buf_size;
  while ( buf_size < max_buf_size )
    {
      step = buf_size;
      memory_bandw = get_bandw(step);
      fprintf(file, "MEMORY_BANDW %d %e\n", step, memory_bandw);
      
      step = buf_size + (buf_size / 4);
      memory_bandw = get_bandw(step);
      fprintf(file, "MEMORY_BANDW %d %e\n", step, memory_bandw);

      step = buf_size + (buf_size / 2);
      memory_bandw = get_bandw(step);
      fprintf(file, "MEMORY_BANDW %d %e\n", step, memory_bandw);

      step = buf_size + (3 * buf_size / 4);
      memory_bandw = get_bandw(step);
      fprintf(file, "MEMORY_BANDW %d %e\n", step, memory_bandw);

      buf_size *= 2;
    }
  memory_bandw = get_bandw(max_buf_size);
  fprintf(file, "MEMORY_BANDW %d %e\n", max_buf_size, memory_bandw);

  // measure event overhead
  double event_ovrhd = get_event_ovrhd();
  fprintf(file, "EVENT_OVRHD %e\n", event_ovrhd);
  cerr << "Overhead numbers written to file 'ovrhds.out'." << endl;
  fclose(file);
}
