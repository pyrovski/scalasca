/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/** @internal
 *
 *  @file       pomp2_fwrapper_base.c
 *  @status     alpha
 *
 *  @maintainer Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 *  @brief      Basic fortan wrappers calling the C versions.*/

#include <stdlib.h>
#include <string.h>

#define POMP2_Assign_handle_U		POMP2_ASSIGN_HANDLE
#define POMP2_Finalize_U		POMP2_FINALIZE
#define POMP2_Init_U			POMP2_INIT
#define POMP2_Off_U			POMP2_OFF
#define POMP2_On_U			POMP2_ON
#define POMP2_Begin_U			POMP2_BEGIN
#define POMP2_End_U			POMP2_END

#define POMP2_Assign_handle_L		pomp2_assign_handle
#define POMP2_Finalize_L		pomp2_finalize
#define POMP2_Init_L			pomp2_init
#define POMP2_Off_L			pomp2_off
#define POMP2_On_L			pomp2_on
#define POMP2_Begin_L			pomp2_begin
#define POMP2_End_L			pomp2_end

#include "opari2/pomp2_lib.h"
#include "epk_fwrapper_def.h"

extern int pomp2_tracing;

void FSUB(POMP2_Assign_handle)(POMP2_Region_handle* regionHandle, char* ctc_string, int ctc_string_len) {
  char *str;
  str=(char*) malloc((ctc_string_len+1)*sizeof(char));
  strncpy(str,ctc_string,ctc_string_len);
  str[ctc_string_len]='\0';
  POMP2_Assign_handle(regionHandle,str);
  free(str);
}

/* *INDENT-OFF*  */
void FSUB(POMP2_Finalize)() {
  POMP2_Finalize();
}

void FSUB(POMP2_Init)() {
  POMP2_Init();
}

void FSUB(POMP2_Off)() {
  pomp2_tracing = 0;
}

void FSUB(POMP2_On)() {
  pomp2_tracing = 1;
}

void FSUB(POMP2_Begin)(POMP2_Region_handle* regionHandle) {
  POMP2_Begin(regionHandle);
}

void FSUB(POMP2_End)(POMP2_Region_handle* regionHandle) {
  POMP2_End(regionHandle);
}

