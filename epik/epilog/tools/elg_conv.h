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

#ifndef _ELG_CONF_H
#define _ELG_CONF_H

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include <sys/types.h>

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - EPILOG Converter Backend Interface
 *
 *-----------------------------------------------------------------------------
 */

/*
 * Utility module
 */

EXTERN int is_mpi_collective(const char* str);
EXTERN void *x_malloc_impl(size_t size, char* file, int line);
EXTERN void *x_realloc_impl(void *ptr, size_t size, char* file, int line);

#define x_malloc(size) x_malloc_impl(size, __FILE__, __LINE__)
#define x_realloc(ptr, size) x_realloc_impl(ptr, size, __FILE__, __LINE__)

/*
 * Region Stack Module
 */

#define STACK_SIZ 2048

typedef struct {
  elg_ui4 reg;
  elg_d8  tim;
} StackNode;

typedef struct {
  StackNode* vals;
  StackNode** idxs;
} RegStack;

extern RegStack mystack;

EXTERN void stk_push(elg_ui4 l, elg_ui4 r, elg_d8 t);
EXTERN StackNode* stk_pop(elg_ui4 l);
EXTERN StackNode* stk_top(elg_ui4 l);

/*
 * String Table Module
 */

extern int numstring;
extern char** stringtab;

/*
 * File Table Module
 */

extern int numfile;
extern char** filetab;

/*
 * SCL Table Module
 */

typedef struct {
  int fid;
  int line;
} SCL;

extern int numscl;
extern int sclid;
extern SCL* scltab;

/*
 * Activity Table Module
 */

typedef struct {
  elg_ui4 id;
  const char* name;
} Activity;

extern char* rtypenames[];
extern int numact;
extern Activity* acttab;
extern elg_ui4 maxact;

extern int act_elg;
extern int act_omp;
extern int act_ompsync;
extern int act_preg;
extern int act_idle;
extern int act_usr;
extern int act_mpi;
extern int need_usr;
extern int is_pattern_trace;

/*
 * Callsite Table Module
 */

typedef struct {
  elg_ui4 csid;
  elg_ui4 erid;
  int sclid;
} Csite;

extern int numcsite;
extern Csite* csitetab;

/*
 * State Table Module
 */

typedef struct {
  elg_ui4 act;
  elg_ui4 id;
  const char* name;
  int sclid;
  elg_ui1 type;
} State;

extern int numstate;
extern int capstate;
extern State* statetab;
extern elg_ui4 maxstate;

extern int state_tracing;
extern int state_off;
extern int state_pregion;
extern int state_idle;
extern int state_user;

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
EXTERN int is_mpi_init_fin(elg_ui4 r);
#endif

/*
 * GlobalOp Table Module
 */

typedef struct {
  elg_ui4 id;
  const char* name;
  elg_ui1 rtype;
} Glop;

extern int numglop;
extern int capglop;
extern Glop* gloptab;

EXTERN int is_glop(elg_ui4 id);

/*
 * Groups Module
 */

typedef struct grp {
  char* name;
  int id;
  int min_id;
  int num_member;
  struct grp* member;
} Group;

#define MAXMACHINE 8
extern int nummachines;
extern Group machine[MAXMACHINE];
extern const char *group_descr[];

extern unsigned int grpid;
extern int totallocs;
extern int totalranks;
extern int *locmap;
extern int *locremap;

typedef struct {
  Group* mach;
  Group* node;
  Group* proc;
  Group* thrd;
  char lstr[64];
} Location;

extern int isthreaded;
extern int numlocs;
extern Location* loctab;
extern unsigned int samplegroup;

#define MAX_THREADS 256

/*
 * Communicator Table Module
 */

typedef struct {
  elg_ui1 mode;
  int numbyte;
  elg_ui1* bits;
  unsigned int *array;
  int numarray;
} Comm;

extern int numcomm;
extern int maxcomm;
extern Comm* commtab;

EXTERN unsigned int get_comm_loc(int l);

/*
 * MPI group table (new scheme)
 */

typedef struct {
  elg_ui1 mode;
  int numranks;
  unsigned int *ranks;
} MpiGroup;

extern int numMpiGroups;
extern int maxMpiGroups;
extern MpiGroup* mpiGroupTab;

/*
 * MPI Communicator table (new scheme)
 */

typedef struct {
  elg_ui4 group;
} MpiComm;

extern int numMpiComms;
extern int maxMpiComms;
extern MpiComm* mpiCommTab;

/*
 * Metrics table
 */

typedef struct {
  const char *name;
  const char *descr;
  elg_ui1 type;
  elg_ui1 mode;
  elg_ui1 iv;
} Metric;

extern int nummet;
extern int maxmet;
extern Metric* mettab;

/*
 * Windows
 */

extern int numwin;
extern int* wintab;

/*
 * Topplogy Table Module
 */

typedef struct {
  elg_ui4 cid;
  elg_ui4 tnid;
  elg_ui1 ndims;
  elg_ui4 numlocs;
  elg_ui4 *sizes;
  elg_ui1 *periods;
  elg_ui4 *coords;
  elg_ui4 *dimids;
} Topology;

extern int numtopol;
extern Topology** topoltab;

/*
 * Conversion interface
 */

typedef struct {
  off_t   done;
  int     numcont;
  char*   strpos;
  elg_ui1 type;
} MYDATA;

extern int genompglop;
extern int genmpiglop;
extern int writeOMP;
extern int addidle;

EXTERN void conv_write_definition_records();
EXTERN void conv_setup();

#endif
