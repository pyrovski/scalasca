/****************************************************************************
**  KOJAK                                                                  **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "vtotf_handler.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char** argv) {
  const char* infile = 0;
  char* outfile = 0;
  int buffersize= 4*1024;
  int ch;
  int eflag = 0;

  OTF_FileManager* manager;
  OTF_Reader* reader;
  OTF_HandlerArray* handlers;
  
  VTOTF_Control fha;
  initOTFHandle(&fha);

  /* argument handling */
  while ((ch = getopt(argc, argv, "h")) != EOF) {
    switch (ch) {
    case 'h':
    case '?': eflag = 1;
              break;
    }
  }
  switch (argc - optind) {
  case 1: infile  = argv[optind];
          outfile = malloc(strlen(infile)+1);
          assert(outfile);
          strcpy(outfile, infile);
          strcpy(outfile+strlen(outfile)-3, "elg");
          break;
  case 2: infile  = argv[optind];
          outfile = argv[optind+1];
          break;
  default: eflag = 1;
          break;
  }
  if (eflag) {
    fprintf(stderr, "usage: vtotf2elg infile.otf [outfile.elg]\n");
    exit(0);
  }

  /* ELG init and basic header */
  fha.elgout = ElgOut_open(outfile, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);

  /* open OTF filemanager and Reader */
  manager = OTF_FileManager_open(50);
  assert(manager);
  reader = OTF_Reader_open(infile, manager);
  assert(reader);
  OTF_Reader_setBufferSizes(reader, buffersize);

  /* init handlers */
  handlers = setupOTFHandlers(&fha);

  /* read OTF records */
  OTF_Reader_readDefinitions(reader, handlers);
  completeDefinitionRecords(&fha);
  OTF_Reader_setRecordLimit(reader, 1);
  while ( OTF_Reader_readEvents(reader, handlers) > 0 ) /*nothing*/;

  /* OTF finalize */
  OTF_Reader_close(reader);
  OTF_HandlerArray_close(handlers);
  OTF_FileManager_close(manager);

  /* ELG finalize */
  ElgOut_close(fha.elgout);
  return 0;
}
