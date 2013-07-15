/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epk_archive.h"
#include "epk_conf.h"
#include "elg_error.h"
#include "esd_unify.h"
#include "esd_report.h"
#include "epk_memory.h"


/*----------------------------------------------------------------------------
 * Function prototypes
 *--------------------------------------------------------------------------*/

void convert_defs(const char* esdname);
void process_file(EsdUnifyData* data, const char* esdname);
void write_definitions(EsdUnifyData* data, /*const*/ char* repname);
long fsize(FILE* fp);


/*
 *----------------------------------------------------------------------------
 * esd2cube
 *
 * Usage: esd2cube epik.esd
 *
 * Reads global definition file and writes contents with cubew.
 *----------------------------------------------------------------------------
 */

int main(int argc, char** argv)
{
  // Validate arguments
  if (argc > 2)
    elg_error_msg("Wrong number of arguments\n"
                  "Usage: esd2cube [epik.esd]");

  convert_defs((argc>1)?argv[1]:"epik.esd");

  return EXIT_SUCCESS;
}


void convert_defs(const char* esdname)
{
  // Build report file name
  char* dirname = epk_archive_filename(EPK_TYPE_DIR, esdname);
  char* repname = (char*)malloc(strlen(dirname) + 11);
  if (strlen(dirname)==0) dirname=".";
  sprintf(repname, "%s/epik.cube", dirname);

  // Create unification data object
  EsdUnifyData* data = esd_unify_create();

  // Load global definitions file
  process_file(data, esdname);

  // Write definitions
  write_definitions(data, repname);

  esd_unify_clear_maps(data);

  // Release unification data object
  esd_unify_free(data);
}


void process_file(EsdUnifyData* data, const char* inname)
{
  buffer_t buffer;
  // Open definitions file
  FILE* infile = fopen(inname, "rb");
  if (!infile)
    elg_error_msg("Could not open file \"%s\".", inname);

  // Allocate buffer
  long size = fsize(infile);
  if (size < 0)
    elg_error_msg("Could not determine size of file \"%s\".", inname);
  buffer = (buffer_t)malloc(size);
  if (!buffer)
    elg_error_msg("Not enough memory for buffer of %d bytes.", size);

  elg_cntl_msg("%9uk: Reading %d bytes of definitions into buffer.", epk_memusage(), size);

  // Read definition data & close file
  if (fread(buffer, size, 1, infile) != 1)
    elg_error_msg("Error reading file \"%s\".", inname);
  fclose(infile);

  elg_cntl_msg("%9uk: Unifying definitions buffer.", epk_memusage());

  // Unify data
  esd_unify_buffer(data, buffer, size, -1, ESD_UNIFY_FULL);

  elg_cntl_msg("%9uk: Unified definitions buffer.", epk_memusage());

  // Release resources
  /*free(buffer);*/
}


void write_definitions(EsdUnifyData* data, /*const*/ char* repname)
{
  // Write definitions
  esd_report_open(repname);
  elg_cntl_msg("%9uk: Writing report.", epk_memusage());
  esd_report_defs(data, 0);
  elg_cntl_msg("%9uk: Closing report.", epk_memusage());
  esd_report_close();
  elg_cntl_msg("%9uk: Closed report.", epk_memusage());
}


long fsize(FILE* fp)
{
  // Determine file size
  long current = ftell(fp);
  fseek(fp, 0, SEEK_END);
  long result = ftell(fp);
  fseek(fp, current, SEEK_SET);

  return result;
}
