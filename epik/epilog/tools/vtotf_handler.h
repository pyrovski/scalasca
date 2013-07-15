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

#ifndef VTOTF_HANDLER_H
#define VTOTF_HANDLER_H

#include "otf.h"
#include "elg_rw.h"
#include "elg_dvector.h"

typedef struct {
  elg_ui4 fnid;
  elg_ui4 line;
} SCL;

typedef struct {
  uint64_t num;
  uint64_t ticksPerSecond;
  ElgOut* elgout;
  elg_ui4 stringid;
  elg_ui4 tracinggid;
  int numloc;
  Dvector funcgrouptab;
  Dvector scltab;
  Dvector comtab;
  Dvector vtapitab;
  elg_ui4* c_com;
  elg_ui4* c_root;
  elg_ui4* c_sent;
  elg_ui4* c_revd;
  void* udata;
} VTOTF_Control;

OTF_HandlerArray* setupOTFHandlers(VTOTF_Control* fha);

void completeDefinitionRecords(VTOTF_Control* fha);

void initOTFHandle(VTOTF_Control* fha);

int handleDefTimerResolution( void* userData, uint32_t stream,
  uint64_t ticksPerSecond );

int handleDefProcess( void* userData, uint32_t stream, uint32_t process,
  const char* name, uint32_t parent );

int handleDefProcessGroup( void* userData, uint32_t stream,
  uint32_t procGroup, const char* name, uint32_t numberOfProcs,
  const uint32_t* procs );

int handleDefFunction( void* userData, uint32_t stream, uint32_t func,
  const char* name, uint32_t funcGroup, uint32_t source );

int handleDefFunctionGroup( void* userData, uint32_t stream,
  uint32_t funcGroup, const char* name );

/*
int handleDefCollectiveOperation( void* userData, uint32_t stream,
  uint32_t collOp, const char* name, uint32_t type );
*/

int handleDefCounter( void* userData, uint32_t stream, uint32_t counter,
  const char* name, uint32_t properties, uint32_t counterGroup,
  const char* unit );

int handleDefCounterGroup( void* userData, uint32_t stream,
  uint32_t counterGroup, const char* name );

int handleDefScl( void* userData, uint32_t stream, uint32_t source,
  uint32_t sourceFile, uint32_t line );

int handleDefSclFile( void* userData, uint32_t stream,
  uint32_t sourceFile, const char* name );

int handleEnter( void* userData, uint64_t time, uint32_t function,
  uint32_t process, uint32_t source );

int handleLeave( void* userData, uint64_t time, uint32_t function,
  uint32_t process, uint32_t source );

int handleSendMsg( void* userData, uint64_t time, uint32_t sender,
  uint32_t receiver, uint32_t group, uint32_t type, uint32_t length,
  uint32_t source );

int handleRecvMsg( void* userData, uint64_t time, uint32_t recvProc,
  uint32_t sendProc, uint32_t group, uint32_t type, uint32_t length, 
  uint32_t source );

int handleCounter( void* userData, uint64_t time, uint32_t process,
  uint32_t counter, uint64_t value );

int handleCollectiveOperation( void* userData, uint64_t time,
  uint32_t process, uint32_t collective, uint32_t procGroup,
  uint32_t rootProc, uint32_t sent, uint32_t received, uint64_t duration, 
  uint32_t source );

int handleBeginProcess( void* userData, uint64_t time, uint32_t process );

int handleEndProcess( void* userData, uint64_t time, uint32_t process );

#endif
