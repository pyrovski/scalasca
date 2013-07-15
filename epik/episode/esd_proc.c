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

/* XXXX EsdProc should manage EsdThrd objects */

#include "esd_proc.h"
#include "esd_gen.h"
#include "epk_archive.h"
#include "epk_idmap.h"
#include "esd_unify.h"
#include "esd_paths.h"
#include "esd_report.h"

#include "elg_pform.h"
#include "elg_error.h"
#include "epk_conf.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if (defined (ELG_MPI) || defined (ELG_OMPI))
#include <mpi.h>
#include "epk_mpireg.h"

EXTERN MPI_Comm esd_comm_world;
#endif
/* lower three PATH_TYPE bits reserved for EPK_MPI_TYPE comm/sync! */
#define PATH_TYPE__NONE 0
#define PATH_TYPE__OMP  8 /* XXXX OMP_HACK */
#define PATH_TYPE__FILE 16 /* XXXX MPI_File I/O */

#define CHUNKSIZE (4 * 1024 * 1024)


/* number of performance metrics (excluding time) */
EXTERN int esd_num_metrics;

typedef struct 
{
    EsdPaths_t* paths;          /* callpaths management object */
} ElgThrd;
/* vector of thread objects */
EXTERN ElgThrd** thrdv;

EsdProc* EsdProc_open( void )
{
  size_t bsize = epk_str2size(epk_get(ESD_BUFFER_SIZE));
  char tmp[PATH_MAX];
  EsdProc *proc;

  proc = (EsdProc*) malloc(sizeof(EsdProc));
  if ( proc == NULL )
    elg_error();

  /* temporary file basename for local process definitions */  
  sprintf(tmp, "%s/%s.%lX.%u", epk_get(EPK_LDIR), epk_get(EPK_TITLE),
                               elg_pform_node_id(), getpid());
    
  proc->gen = EsdGen_open(tmp, ELG_NO_ID, bsize);

  proc->arc_name = NULL; /* not known yet */
  proc->tmp_name = (char*)calloc(strlen(tmp) + 1, sizeof(char*));
  if ( proc->tmp_name == NULL )
    elg_error();

  strcpy(proc->tmp_name, EsdGen_get_name(proc->gen));

  elg_cntl_msg("Process object created");
  
  return proc;
}

static int move_file(char* ifile, char *ofile)
{ /* rename(2) can only be used for files on the same filesystem */
  int ch, ret;
  FILE *fi = fopen(ifile, "r");
  FILE *fo = fopen(ofile, "w");
  if (!fi) elg_error_msg("Failed to open %s for reading", ifile);  
  if (!fo) elg_error_msg("Failed to open %s for writing", ofile);  
  while (((ch = getc(fi))!=EOF) && ((ret=putc(ch,fo))!=EOF)) /*done*/;
  if (ch!=EOF) elg_error_msg("Failed to write to %s", ofile);
  if (ret==EOF) elg_error_msg("Failed to read from %s", ifile);
  if (fclose(fi)) elg_error_msg("Failed to close %s", ifile);
  if (fclose(fo)) elg_error_msg("Failed to close %s", ofile);
  /* could check ELG_CLEAN do decide whether to remove file */
  if (unlink(ifile)) elg_error_msg("Failed to remove %s", ifile);
  return 0;
}


void EsdProc_close(EsdProc* proc, elg_ui4 rank)
{
  EsdGen_close(proc->gen);

  if (!proc->arc_name) {
      /* destination for process definitions */
      proc->arc_name = epk_archive_rankname(EPK_TYPE_ESD, epk_archive_get_name(), rank);
      epk_archive_create(EPK_TYPE_ESD);

      /* move process definitions file to destination */
      if (move_file(proc->tmp_name, proc->arc_name) == 0)
          elg_cntl_msg("Moved definitions file %s to %s",
                        proc->tmp_name, proc->arc_name);
      else
          elg_error_msg("Failed to move definitions file %s to %s",
                        proc->tmp_name, proc->arc_name);
  }

  elg_cntl_msg("Process object deleted");
  free(proc->tmp_name);
  free(proc->arc_name);
  free(proc);
}


ElgOut* EsdProc_opendef ()
{
  /* Open global definitions file */
  char*   dirname = epk_archive_get_name();
  char*   outname = epk_archive_filename(EPK_TYPE_ESD, dirname);
  ElgOut* outfile = ElgOut_open(outname, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);
  if (!outfile)
    elg_error_msg("Could not create file \"%s\".", outname);

  /* Release resources */
  free(outname);

  return outfile;
}

ElgOut* EsdProc_openmap ()
{
  /* Open mapping file */
  char*   dirname = epk_archive_get_name();
  char*   outname = epk_archive_filename(EPK_TYPE_EMP, dirname);
  ElgOut* outfile = ElgOut_open(outname, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);
  if (!outfile)
    elg_error_msg("Could not create file \"%s\".", outname);

  /* Release resources */
  free(outname);

  return outfile;
}

EXTERN int esd_summary;
EXTERN int esd_tracing;

/* local->global callpath identifier map vector */
static size_t   esd_idmapsz = 0;
static elg_ui4* esd_idmapv = NULL;
static elg_ui4  esd_total_paths = 0;
static elg_ui4  esd_unknowns = 0;

/* vectors of callpath types (MPI collective and pt2pt) */
static elg_ui1* esd_pathtypev = NULL;

/* determine which callpaths have MPI metrics of various kinds */
static void determine_pathtypes(EsdUnifyData* udata)
{
    /* determine MPI & OMP region descid */
    elg_ui4 mpi_descid, omp_descid;
    for (mpi_descid=0; mpi_descid<epk_vector_size(udata->strings); mpi_descid++) {
        if (strcmp(epk_vector_at(udata->strings,mpi_descid), "MPI") == 0) break;
    }
    for (omp_descid=0; omp_descid<epk_vector_size(udata->strings); omp_descid++) {
        if (strcmp(epk_vector_at(udata->strings,omp_descid), "OMP") == 0) break;
    }
    elg_cntl_msg("MPI region descid=%u OMP region descid=%u", mpi_descid, omp_descid);

    /* determine MPI collective and pt2pt callpaths */
    esd_pathtypev = calloc(esd_total_paths, sizeof(elg_ui1));
    if (esd_pathtypev) {
        elg_ui4 pathid;
        elg_ui4 colls=0, pt2pt=0, mfile=0, other=0, pregs=0;
        for (pathid=0; pathid<esd_total_paths; pathid++) {
            UnifyCpath*  ucpath = epk_vector_at(udata->cpaths, pathid);
            UnifyRegion* uregion = epk_vector_at(udata->regions, ucpath->rid);
            if (uregion->descid == omp_descid) { /* XXXX OMP_HACK */
                const char* omp_parallel = "!$omp parallel";
                char* regnname = epk_vector_at(udata->strings, uregion->nameid);
                if (strncmp(regnname, omp_parallel, 14) == 0) {
                    esd_pathtypev[pathid] = PATH_TYPE__OMP;
                    elg_cntl_msg("cpath[%d]: %s eventtype=OMP rid=%d", pathid, regnname, ucpath->rid);
                    pregs++; /* OMP paths to parallel regions */
                }
#if (defined (ELG_MPI) || defined(ELG_OMPI))
            } else if (uregion->descid == mpi_descid) {
                char* regnname = epk_vector_at(udata->strings, uregion->nameid);
                elg_ui1 eventtype = epk_mpi_eventtype(regnname);
                elg_cntl_msg("cpath[%d]: %s eventtype=%u", pathid, regnname, eventtype);
                esd_pathtypev[pathid] = eventtype;
                if (eventtype == EPK_MPI_TYPE__NONE)
                    other++; /* MPI paths without additional metrics */
                else if (eventtype == EPK_MPI_TYPE__COLLECTIVE)
                    colls++; /* MPI paths with collective metrics */
                else
                    pt2pt++; /* MPI paths with send and/or receive metrics */
                /* final check for MPI File I/O routines */
                if (strncmp(regnname, "MPI_File_", 9) == 0) {
                    if ((strncmp(regnname, "MPI_File_write",14) != 0) &&
                        (strncmp(regnname, "MPI_File_i", 10) != 0) &&  /* iread/iwrite */
                        (strncmp(regnname, "MPI_File_read",13) != 0)) continue;
                    esd_pathtypev[pathid] = PATH_TYPE__FILE;
                    elg_cntl_msg("cpath[%d]: %s eventtype=FILE", pathid, regnname);
                    mfile++; /* MPI paths with file I/O metrics */
                }
                /* Waits can also complete non-blocking MPI File I/O! */
                if (strncmp(regnname, "MPI_Wait", 8) == 0) {
                    esd_pathtypev[pathid] += PATH_TYPE__FILE;
                    elg_cntl_msg("cpath[%d]: %s eventtype=FILE", pathid, regnname);
                    mfile++; /* MPI paths with file I/O metrics */
                }
#endif
            } else esd_pathtypev[pathid] = PATH_TYPE__NONE;
        }
        elg_cntl_msg("Got %d collective %d pt2pt %d file %d other MPI callpaths",
                    colls, pt2pt, mfile, other);
        elg_cntl_msg("Got %d OMP parallel region callpaths", pregs);
    }
}


#if (defined (ELG_MPI) || defined(ELG_OMPI))
/** @internal
 * Distributed communicator record comparison function.
 */
static int comm_cmp(const void* value, const void* item)
{
  UnifyComDist* v = (UnifyComDist*)value;
  UnifyComDist* i = (UnifyComDist*)item;

  if (v->comid < i->comid)
    return -1;
  else if (v->comid > i->comid)
    return 1;

  return 0;
}
#endif


/**
 * This function performs a hierarchical unification of the local definitions
 * of size @a defBytes provided in @a defBuffer and creates local-to-global
 * identifier mappings on each process. In the case of MPI or hybrid MPI/OpenMP,
 * it is collective over the communicator @a esd_comm_world.
 *
 * The unification algorithm performs multiple steps:
 *   -# <b>Generate MPI comminicator identifier mapping</b><br>
 *      This is required as the first step since Cartesian topology and MPI RMA
 *      window definitions reference MPI communicators and their global IDs are
 *      required for the hierarchical unification in step 3.
 *   -# <b>Adjust the MPI communicator references</b><br>
 *      In this step, all references to MPI communicators in other definition
 *      records (i.e., Cartesian topology and MPI RMA window definitions) in
 *      are adjusted to use the global IDs determined in the previous step.
 *   -# <b>Generate a global set of definitions on rank 0</b><br>
 *      Definitions are iteratively merged in a tree-like fashion. That is, in
 *      the first iteration, each process communicates with its direct neighbor.
 *      Then, the distance (i.e., rank offset) between senders and receivers is
 *      doubled with each iteration, as shown in the following 8-process
 *      example:
 *      @verbatim
 *        Iteration 1:
 *          0 <- 1    2 <- 3    4 <- 5    6 <- 7
 *        Iteration 2:
 *          0 <------ 2         4 <------ 6
 *        Iteration 3:
 *          0 <---------------- 4
 *      @endverbatim
 *      Finally, rank zero has the full set of global definitions, excluding
 *      MPI communicators (which will be unified separately in the last step).
 *   -# <b>Broadcast the global definitions to every rank</b><br>
 *      To avoid sending unnecessary per-process data, location and
 *      topology data (which is always process-local) is excluded from this
 *      step.
 *   -# <b>Create local-to-global identifier mappings</b><br>
 *      This is done by unifying the local set of definitions with the global
 *      definitions just received. Afterwards, the location mapping needs to
 *      be adjusted since no global location information is available.
 *   -# <b>Unify MPI communicators</b><br>
 *      Since MPI communicators were excluded from the hierarchical unification
 *      performed in the third step, they have to be processed separately. This
 *      is done using a series of calls to MPI_Gather, collecting the group
 *      information for each communicator.
 * In the end, the unification data object containing the global set of
 * definitions as well as the local-to-global identifier mappings is returned
 * on each process.
 *
 * @param defBuffer Local definition buffer
 * @param defBytes  Size of local definition buffer
 * @param myRank    MPI rank (0 for serial/OpenMP)
 * @param numRanks  Total number of ranks (1 for serial/OpenMP)
 *
 * @return Definition data object containing global definitions and
 *         local-to-global identifier mapping
 */
EsdUnifyData* EsdProc_unify_defs(const buffer_t defBuffer,
                                       size_t   defBytes,
                                       int      myRank,
                                       int      numRanks)
{
  EsdUnifyData* unifyData = NULL;

#if defined(ELG_MPI) || defined(ELG_OMPI)
  unsigned int  iter;
  unsigned int  maxIter;
  unsigned int  globalId;
  unsigned int  numLocs;
  unsigned int  maxCommSelf;
  unsigned int  localOffset;
  unsigned int  firstSelfId;
  unsigned int  selfId;
  unsigned int* buffer;
  EpkIdMap*     mapTable;
  elg_ui4*      ranks;
  void**        ptr;
  void**        end;

/*** TEMP: time measurement
  double start_time;
*/
#endif   /* ELG_MPI || ELG_OMPI */


  /*--- STEP 0: Initialize local unification object -----------------------*/

  unifyData = esd_unify_create();
  esd_unify_buffer(unifyData, defBuffer, defBytes, myRank, ESD_UNIFY_PARTIAL);
#if defined(ELG_MPI) || defined(ELG_OMPI)
  /* Determine number of local locations (needed in step 5) */
  numLocs = epk_vector_size(unifyData->locs);
#endif   /* ELG_MPI || ELG_OMPI */


  /*--- SPECIAL CASE: Only a single rank ----------------------------------*/

  if (1 == numRanks) {
    elg_cntl_msg("Unify(1-6): only a single rank");

#if defined(ELG_MPI) || defined(ELG_OMPI)
    /* In case of MPI or hybrid MPI/OpenMP with a single process, only one
       group (SELF) exists. It is therefore safe to use this group for all
       communicators. Also, there is a 1:1 mapping of communicator IDs. */
    mapTable = esd_unify_get_map(unifyData, ESD_UNIFY_COMMAP);
    globalId = esd_unify_add_group(unifyData, ELG_NO_ID,
                                   ELG_GROUP_WORLD | ELG_GROUP_SELF, 0, NULL);
    maxIter  = epk_vector_size(unifyData->dist_coms);
    for (iter = 0; iter < maxIter; ++iter) {
      esd_unify_add_comm(unifyData, iter, globalId);
      epk_idmap_add(mapTable, iter, iter);
    }
#endif   /* ELG_MPI || ELG_OMPI */

    return unifyData;
  }


#if defined(ELG_MPI) || defined(ELG_OMPI)

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 1: Create MPI communicator ID mapping ------------------------*/

  if (0 == myRank)
    elg_cntl_msg("Unify(1): create MPI communicator ID mapping");

  /* Determine maximum number of different COMM_SELF references */
  PMPI_Allreduce(&unifyData->com_self, &maxCommSelf, 1, MPI_UNSIGNED,
                 MPI_MAX, esd_comm_world);
  if (0 == myRank)
    elg_cntl_msg("Unify(1): No. of COMM_SELF references: %u", maxCommSelf);

  /* Determine the first global communicator identifier of this process.
     This is essentially an exclusive prefix sum over the 'root count'.
     For backwards compatibility with non-MPI 2.x compliant MPIs, we use
     MPI_Scan here. As a side effect, localOffset of rank n-1 holds the
     global ID of the first COMM_SELF communicator. */
  elg_cntl_msg("Unify(1): Number of 'root' communicators: %u",
                unifyData->com_root);
  PMPI_Scan(&unifyData->com_root, &localOffset, 1, MPI_UNSIGNED, MPI_SUM,
            esd_comm_world);
  firstSelfId = localOffset;
  PMPI_Bcast(&firstSelfId, 1, MPI_UNSIGNED, numRanks - 1, esd_comm_world);
  localOffset -= unifyData->com_root;
  elg_cntl_msg("Unify(1): First global communicator ID: %u", localOffset);
  if (0 == myRank)
    elg_cntl_msg("Unify(1): First global ID for COMM_SELF references: %u",
                 firstSelfId);

  /* Gather all communicator ID offsets */
  buffer = (unsigned int*)malloc(numRanks * sizeof(unsigned int));
  if (!buffer)
    elg_error_msg("Failed to malloc communicator offset buffer");
  PMPI_Allgather(&localOffset, 1, MPI_UNSIGNED,  buffer, 1, MPI_UNSIGNED,
                 esd_comm_world);

  /* Create communicator ID mapping */
  selfId   = firstSelfId;
  mapTable = esd_unify_get_map(unifyData, ESD_UNIFY_COMMAP);
  maxIter  = epk_vector_size(unifyData->dist_coms);
  for (iter = 0; iter < maxIter; ++iter) {
    UnifyComDist* entry = (UnifyComDist*)epk_vector_at(unifyData->dist_coms,
                                                       iter);

    /* Map identifier */
    if (1 == entry->size)
      entry->comid = selfId++;
    else
      entry->comid += buffer[entry->root];

    epk_idmap_add(mapTable, entry->lcomid, entry->comid);
  }

  /* Sort communicator entries according to their global IDs */
  epk_vector_sort(unifyData->dist_coms, comm_cmp);

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(1) - Comm ID mapping: %0.3fs", elg_pform_wtime() - start_time);
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 2: Adjust MPI communicator ID references ---------------------*/

  if (0 == myRank)
    elg_cntl_msg("Unify(2): adjusting MPI communicator references");

  /* Process cartesian topologies */
  ptr = epk_vector_begin(unifyData->tops);
  end = epk_vector_end(unifyData->tops);
  while (ptr != end) {
    UnifyCart* cart = (UnifyCart*)(*ptr);
    cart->comid = epk_idmap_get(mapTable, cart->comid);
    ++ptr;
  }

  /* Process MPI RMA windows */
  ptr = epk_vector_begin(unifyData->wins);
  end = epk_vector_end(unifyData->wins);
  while (ptr != end) {
    UnifyWin* window = (UnifyWin*)(*ptr);

    window->comid = epk_idmap_get(mapTable, window->comid);
    ++ptr;
  }

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(2) - Adjust comm refs: %0.3fs", elg_pform_wtime() - start_time);
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 3: Generate global set of definitions on rank 0 --------------*/

  /* #iterations for hierarchical unification is ceil(log2(numRanks));
     if numRanks is a power of 2, the loop counts one iteration too much */
  maxIter = 0;
  iter    = numRanks;
  while (iter) {
    ++maxIter;
    iter /= 2;
  }
  if (0 == (numRanks & (numRanks - 1)))
    --maxIter;
  if (0 == myRank)
    elg_cntl_msg("Unify(3): hierarchical unifcation, %d iterations", maxIter);

  /* Hierarchical unification */
  esd_unify_clear_maps(unifyData);
  for (iter = 0; iter < maxIter; ++iter) {
    int dist   = 1 << iter;         /* Rank offset sender/receiver: 2^iter */
    int step   = 2 * dist;          /* Rank offset of two receivers: 2*dist */
    int source = myRank + dist;     /* Sender rank */

    /* Am I a receiver and is there a corresponding sender? */
    if ((0 == myRank % step) && (source < numRanks)) {
      MPI_Status recvStatus;
      int        recvBytes;
      buffer_t   recvBuffer;

      /* Receive definition data from peer process */
      PMPI_Probe(source, 42, esd_comm_world, &recvStatus);
      PMPI_Get_count(&recvStatus, MPI_BYTE, &recvBytes);
      elg_cntl_msg("Unify(3): Iteration %d >> receiving %d bytes from %d",
                   iter, recvBytes, source);
      recvBuffer = (buffer_t)malloc(recvBytes);
      if (!recvBuffer)
        elg_error_msg("Failed to malloc recv buffer");
      PMPI_Recv(recvBuffer, recvBytes, MPI_BYTE, source, 42, esd_comm_world,
                &recvStatus);

      /* Unify received data */
      esd_unify_buffer(unifyData, recvBuffer, recvBytes, -1, ESD_UNIFY_PARTIAL);
      esd_unify_clear_maps(unifyData);

      /* Clean up */
      free(recvBuffer);
    }

    /* Am I a sender? */
    else if (dist == myRank % step) {
      int      dest = myRank - dist;     /* Receiver rank */
      size_t   sendBytes;
      buffer_t sendBuffer;

      /* Send partially unified data */
      esd_unify_serialize_defs(unifyData, &sendBuffer, &sendBytes,
                               ESD_UNIFY_FULL);
      elg_cntl_msg("Unify(3): Iteration %d >> sending %d bytes to %d",
                   iter, sendBytes, dest);
      PMPI_Send(sendBuffer, sendBytes, MPI_BYTE, dest, 42, esd_comm_world);

      /* Clean up */
      free(sendBuffer);
      esd_unify_clear_data(unifyData);
    }
  }

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(3) - hierarchical unify: %0.3fs", elg_pform_wtime() - start_time);
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 4: Broadcast global definitions to every rank ----------------*/

  if (0 == myRank) {
    size_t       sendBytes;
    buffer_t     sendBuffer;
    unsigned int msgSize;

    elg_cntl_msg("Unify(4): broadcasting global definition data");

    /* Send global definiton data */
    esd_unify_serialize_defs(unifyData, &sendBuffer, &sendBytes,
                             ESD_UNIFY_PARTIAL);
    elg_cntl_msg("Unify(4): broadcasting %d bytes", sendBytes);
    msgSize = sendBytes;
    PMPI_Bcast(&msgSize, 1, MPI_UNSIGNED, 0, esd_comm_world);
    PMPI_Bcast(sendBuffer, sendBytes, MPI_BYTE, 0, esd_comm_world);

    /* Clean up */
    free(sendBuffer);
  } else {
    unsigned int recvBytes;
    buffer_t     recvBuffer;

    /* Receive global definition data */
    PMPI_Bcast(&recvBytes, 1, MPI_UNSIGNED, 0, esd_comm_world);
    elg_cntl_msg("Unify(4): receiving %d bytes from broadcast", recvBytes);
    recvBuffer = (buffer_t)malloc(recvBytes);
    if (!recvBuffer)
      elg_error_msg("Failed to malloc recv buffer");
    PMPI_Bcast(recvBuffer, recvBytes, MPI_BYTE, 0, esd_comm_world);

    /* Create unification data structure initialized with global definitions */
    esd_unify_buffer(unifyData, recvBuffer, recvBytes, -1, ESD_UNIFY_PARTIAL);
    esd_unify_clear_maps(unifyData);

    /* Clean up */
    free(recvBuffer);
  }

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(4) - broadcast: %0.3fs", elg_pform_wtime() - start_time);
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 5: Create local-to-global ID mappings ------------------------*/

  if (0 == myRank)
    elg_cntl_msg("Unify(5): determine local-to-global ID mappings");

  /* Create mappings by unifying local definition data */
  esd_unify_buffer(unifyData, defBuffer, defBytes, myRank, ESD_UNIFY_FULL);

  /* Determine first global location identifier of this process: This is
     essentially an exclusive prefix sum over the number of locations. For
     backwarnds compatibility with non-MPI 2.x compliant MPIs, we use
     MPI_Scan here. */
  PMPI_Scan(&numLocs, &globalId, 1, MPI_UNSIGNED, MPI_SUM, esd_comm_world);
  globalId -= numLocs;

  /* Overwrite location mapping */
  mapTable = esd_unify_get_map(unifyData, ESD_UNIFY_LOCMAP);
  epk_idmap_clear(mapTable);
  for (iter = 0; iter < numLocs; iter++)
    epk_idmap_add(mapTable, iter, globalId++);

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(5) - create maps: %0.3fs", elg_pform_wtime() - start_time);
  start_time = elg_pform_wtime();
*/

  /*--- STEP 6: Unify MPI communicators -----------------------------------*/

  if (0 == myRank)
    elg_cntl_msg("Unify(6): unify MPI communicators");

  /* Unify communicator groups */
  if (0 == myRank) {
    ranks = (elg_ui4*)malloc(numRanks * sizeof(elg_ui4));
    if (!ranks)
    elg_error_msg("Failed to malloc group rank buffer");
  }

  /* Process non-SELF communicators */
  ptr = epk_vector_begin(unifyData->dist_coms);
  end = epk_vector_end(unifyData->dist_coms);
  for (iter = 0; iter < firstSelfId; ++iter) {
    UnifyComDist* entry = (UnifyComDist*)(*ptr);

    /* Determing local rank in current communicator */
    elg_ui4 lrank = ELG_NO_ID;
    if (ptr != end && entry->comid == iter) {
      lrank = entry->lrank;
      ++ptr;
    }

    /* Gather group information (Barrier used to avoid flooding of rank 0) */
    if (0 == myRank)
      elg_cntl_msg("Unify(6): Collecting group info of global communicator %u",
                   iter);
    PMPI_Barrier(esd_comm_world);
    PMPI_Gather(&lrank, 1, MPI_UNSIGNED, buffer, 1, MPI_UNSIGNED, 0,
                esd_comm_world);

    if (0 == myRank) {
      /* Construct local rank |-> global rank mapping */
      elg_ui1 world_flag = ELG_GROUP_WORLD;
      size_t  count      = 0;
      int     rank;
      for (rank = 0; rank < numRanks; ++rank) {
        if (buffer[rank] != ELG_NO_ID) {
          ++count;
          ranks[buffer[rank]] = rank;
        }
        if (world_flag && buffer[rank] != rank)
          world_flag = 0;
      }

      /* Define MPI group & MPI communicator */
      globalId = esd_unify_add_group(unifyData, ELG_NO_ID, world_flag,
                                     count, ranks);
      esd_unify_add_comm(unifyData, iter, globalId);
    }
  }

  if (0 == myRank) {
    /* Process SELF communicators */
    globalId = esd_unify_add_group(unifyData, ELG_NO_ID, ELG_GROUP_SELF, 0, NULL);
    for (iter = 0; iter < maxCommSelf; ++iter)
      esd_unify_add_comm(unifyData, iter + firstSelfId, globalId);
    free(ranks);
  }
  free(buffer);

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(6) - unify comms: %0.3fs", elg_pform_wtime() - start_time);
*/
#endif   /* ELG_MPI || ELG_OMPI */

  return unifyData;
}


/**
 * Writes the global definition and mapping data provided by @a unifyData to
 * the corresponding files in the experiment archive specified by @a dirname.
 * In the case of MPI or hybrid MPI/OpenMP, this function is collective over
 * the communicator @a esd_comm_world.
 *
 * The algorithm used first collects the mapping data in chunks of CHUNKSIZE
 * bytes on a small number of collector processes using a hierarchical gather
 * algorithm, with the collector processes defined as the first rank providing
 * data for any particular chunk. Then, these chunks are sent to rank zero
 * which writes them to disk. In the end, the global definition file is also
 * written by rank zero.
 *
 * @param dirname   Experiment archive directory
 * @param unifyData Unification data object providing the data to be written.
 * @param myRank    MPI rank (0 for serial/OpenMP)
 * @param numRanks  Total number of ranks (1 for serial/OpenMP)
 */
void EsdProc_write_unify_data(const char*         dirname,
                                    EsdUnifyData* unifyData,
                                    int           myRank,
                                    int           numRanks)
{
  char*   filename;
  ElgOut* defFile;

#if defined(ELG_MPI) || defined(ELG_OMPI)
  FILE*   mapFile;

/*** TEMP: time measurement
  double start_time;
  PMPI_Barrier(esd_comm_world);
  start_time = elg_pform_wtime();
*/
#endif   /* ELG_MPI || ELG_OMPI */


  if (0 == myRank) {
    filename = epk_archive_filename(EPK_TYPE_ESD, dirname);
    defFile  = ElgOut_open(filename, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);
    if (!defFile)
      elg_error_msg("Could not create global definition file \"%s\"", filename);
    free(filename);
  }


  /*--- SPECIAL CASE: Only a single rank ----------------------------------*/

  if (1 == numRanks) {
    elg_cntl_msg("Unify(7): only a single rank");

    /* Write global definitions */
    /* First offset is always 10 due to the EPILOG header */
    ElgOut_write_MAP_OFFSET(defFile, myRank, 10);
    esd_unify_write_defs(unifyData, defFile, ESD_UNIFY_FULL);
    ElgOut_close(defFile);

    /* Write mapping file */
    filename = epk_archive_filename(EPK_TYPE_EMP, dirname);
    defFile  = ElgOut_open(filename, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);
    if (!defFile)
      elg_error_msg("Could not create mapping file \"%s\"", filename);
    free(filename);
    esd_unify_write_maps(unifyData, myRank, defFile);
    ElgOut_close(defFile);

    return;
  }

#if defined(ELG_MPI) || defined(ELG_OMPI)

  /* Create mapping file from rank zero */
  filename = epk_archive_filename(EPK_TYPE_EMP, dirname);
  if (0 == myRank) {
    mapFile = fopen(filename, "wb");
    if (!mapFile)
      elg_error_msg("Could not create mapping file \"%s\"", filename);
  }

  /* Serialize mapping data */
  buffer_t mapBuffer;
  size_t   mapSize;
  esd_unify_serialize_maps(unifyData, myRank, &mapBuffer, &mapSize);
  if (0 != myRank) {     /* Exclude EPILOG header except for rank zero */
    mapBuffer += 10;
    mapSize   -= 10;
  }

  /* This algorithm only works if all processes generate less than
     CHUNKSIZE bytes of mapping data! */
  assert(mapSize <= CHUNKSIZE);

  /* Determine own file offset: This is essentially an exclusice scan, but we
     use MPI_Scan here for compatibility with non-MPI 2.x compliant MPIs */
  long long accumBytes;
  long long tmpSize = mapSize;
  PMPI_Scan(&tmpSize, &accumBytes, 1, MPI_LONG_LONG, MPI_SUM, esd_comm_world);
  unsigned int myOffset = accumBytes - tmpSize;

  /* Determine total number of bytes */
  long long totalBytes = accumBytes;
  PMPI_Bcast(&totalBytes, 1, MPI_LONG_LONG, numRanks - 1, esd_comm_world);

  /* Due to the current specification of the ELG_MAP_OFFSET record, the total
     amount of mapping data must be less than 4 GB! */
  assert(totalBytes <= 0xFFFFFFFFu);

  /* Calculate total number of chunks */
  unsigned int totalChunks = (totalBytes + CHUNKSIZE - 1) / CHUNKSIZE;

  /* Determine own chunk to receive; UNIT_MAX if not a receiver */
  unsigned int myChunk;
  unsigned int tmpChunk = (accumBytes - 1) / CHUNKSIZE;
  PMPI_Scan(&tmpChunk, &myChunk, 1, MPI_UNSIGNED, MPI_MAX, esd_comm_world);

  /* Determine root of own gather */
  int in[2] = { totalChunks - (myOffset / CHUNKSIZE), myRank };
  int out[2];
  PMPI_Scan(&in, &out, 1, MPI_2INT, MPI_MINLOC, esd_comm_world);
  if (out[1] > 0 && myOffset % CHUNKSIZE != 0)
    out[1]--;
  in[0] = out[1];
  PMPI_Scan(&in, &out, 1, MPI_INT, MPI_MAX, esd_comm_world);
  int myRoot = out[0];

  /* Calculate number of bytes to send/copy/receive;
     set myChunk to UINT_MAX if not a receiver */
  unsigned int bytesToRecv = (myChunk + 1) * CHUNKSIZE;
  if (bytesToRecv > totalBytes)
    bytesToRecv = totalBytes;
  bytesToRecv -= myOffset + mapSize;
  unsigned int bytesToSend = mapSize;
  unsigned int bytesToCopy = 0;
  if (myOffset > myChunk * CHUNKSIZE) {
    myChunk = UINT_MAX;
  } else {
    bytesToSend = (myChunk * CHUNKSIZE) - myOffset;
    bytesToCopy = mapSize - bytesToSend;
  }
  bytesToRecv += bytesToCopy;

  /* Set up receive buffer */
  buffer_t recvBuffer = (buffer_t)malloc(CHUNKSIZE);
  if (!recvBuffer)
    elg_error_msg("Failed to malloc recv buffer");
  unsigned int bytesRecvd = 0;
  if (bytesToCopy > 0) {
    memcpy(recvBuffer, mapBuffer + bytesToSend, bytesToCopy);
    bytesRecvd += bytesToCopy;
  }

  /* Hierarchical gather */
  int iter       = 0;
  int activeSend = (bytesToSend > 0);
  int activeRecv = (bytesRecvd < bytesToRecv);
  while (activeSend || activeRecv) {
    int dist   = 1 << iter;         /* Rank offset sender/receiver: 2^iter */
    int step   = 2 * dist;          /* Rank offset of two receivers: 2*dist */
    int source = myRank + dist;     /* Sender rank */

    /* Am I a sender? */
    if (dist == (myRank - myRoot) % step) {
      int dest = myRank - dist;

      /* Send data */
      if (bytesToSend > 0) {
        PMPI_Send(mapBuffer, bytesToSend, MPI_BYTE, dest, 33, esd_comm_world);
        bytesToSend = 0;
      } else {
        PMPI_Send(recvBuffer, bytesRecvd, MPI_BYTE, dest, 33, esd_comm_world);
        bytesRecvd = 0;
      }

      /* Update activity status */
      activeSend = 0;
      if (myChunk == UINT_MAX)
        activeRecv = 0;
    }

    /* Am I a final receiver? */
    if (myChunk < UINT_MAX && bytesRecvd < bytesToRecv) {
      MPI_Status status;
      int        count;

      /* Receive data */
      PMPI_Recv(recvBuffer + bytesRecvd, CHUNKSIZE - bytesRecvd, MPI_BYTE,
                source, 33, esd_comm_world, &status);
      PMPI_Get_count(&status, MPI_BYTE, &count);
      bytesRecvd += count;

      /* Update activity status */
      if (bytesRecvd == bytesToRecv)
        activeRecv = 0;
    }

    /* Am I a intermediate receiver? */
    if (myChunk == UINT_MAX &&
        0 == (myRank - myRoot) % step &&
        bytesRecvd < bytesToRecv) {
      MPI_Status status;
      int        count;

      /* Copy own data to receive buffer */
      if (bytesToSend > 0) {
        memcpy(recvBuffer, mapBuffer, bytesToSend);
        bytesToRecv += bytesToSend;
        bytesRecvd  += bytesToSend;
        bytesToSend  = 0;
        activeSend   = 0;
      }

      /* Receive data */
      PMPI_Recv(recvBuffer + bytesRecvd, CHUNKSIZE - bytesRecvd, MPI_BYTE,
                source, 33, esd_comm_world, &status);
      PMPI_Get_count(&status, MPI_BYTE, &count);
      bytesRecvd += count;
    }

    ++iter;
  }

  /* Write mapping data */
  if (0 == myRank) {
    int chunk = 1;

    fwrite(recvBuffer, bytesRecvd, 1, mapFile);
    while (bytesRecvd < totalBytes) {
      MPI_Status status;
      int        count;

      PMPI_Recv(recvBuffer, CHUNKSIZE, MPI_BYTE, MPI_ANY_SOURCE, chunk,
                esd_comm_world, &status);
      PMPI_Get_count(&status, MPI_BYTE, &count);
      fwrite(recvBuffer, count, 1, mapFile);
      bytesRecvd += count;
      ++chunk;
    }
    fclose(mapFile);
  } else if (myChunk < UINT_MAX) {
    PMPI_Send(recvBuffer, bytesRecvd, MPI_BYTE, 0, myChunk, esd_comm_world);
  }

  /* Gather file offsets */
  unsigned int* offsets = NULL;
  if (0 == myRank) {
    offsets = (unsigned int*)malloc(numRanks * sizeof(unsigned int));
    if (!offsets)
      elg_error_msg("Failed to malloc offsets buffer");
  }
  PMPI_Gather(&myOffset, 1, MPI_UNSIGNED, offsets, 1, MPI_UNSIGNED, 0,
              esd_comm_world);
  if (0 == myRank) {
    /* Adjust for EPILOG header */
    offsets[0] += 10;

    /* Write mapping file offsets */
    int rank;
    for (rank = 0; rank < numRanks; rank++)
        ElgOut_write_MAP_OFFSET(defFile, rank, offsets[rank]);

    /* Write global definitions */
    esd_unify_write_defs(unifyData, defFile, ESD_UNIFY_FULL);
    ElgOut_close(defFile);

    free(offsets);
  }

  /* Clean up */
  if (0 != myRank)
    mapBuffer -= 10;
  free(mapBuffer);
  free(recvBuffer);

/*** TEMP: time measurement
  PMPI_Barrier(esd_comm_world);
  if (0 == myRank)
    elg_warning("Unify(7) - Write defs+maps (multi-root gather, serial I/O): %0.3fs",
                elg_pform_wtime() - start_time);
*/
#endif
}


int EsdProc_unify (EsdProc* proc, elg_ui4 my_rank, elg_ui4 num_ranks, int sync)
{
  static unsigned Unifier = 0; /* default unification on master */
  double start_time = elg_pform_wtime();
  size_t bsize = epk_str2size(epk_get(ESD_BUFFER_SIZE));
  size_t reqbytes = EsdGen_get_bytes(ESDPROC_GEN(proc));
  size_t defbytes = EsdGen_get_held(ESDPROC_GEN(proc));
  buffer_t defbuf = EsdGen_get_data(ESDPROC_GEN(proc));
  size_t maxbytes = reqbytes;
  elg_ui4 frames = esd_frame_depth(thrdv[0]->paths);
  elg_ui4 maxframes = frames;
  elg_ui4 unknowns = esd_path_visits(thrdv[0]->paths, 0);
  elg_ui4 lpaths = esd_paths_size(thrdv[0]->paths);
  elg_ui4 maxpaths = lpaths;
  esd_unknowns = unknowns;

  char *verb_unify = strstr(epk_get(EPK_VERBOSE),"unify");
  if (verb_unify && elg_rank_match(my_rank, verb_unify+strlen("unify")))
      elg_verbosity(1);

  if (my_rank == Unifier) {
      elg_cntl_msg("Unifying...");
      /*if (esd_tracing) epk_archive_create(EPK_TYPE_ELU);*/
  }

#if (defined (ELG_MPI) || defined(ELG_OMPI))
  PMPI_Barrier(esd_comm_world);
  elg_cntl_msg("%d bytes of local definitions (paths=%d, frames=%d, unknowns=%d)",
               reqbytes, lpaths, frames, unknowns);
  PMPI_Allreduce(&reqbytes, &maxbytes, 1, MPI_UNSIGNED, MPI_MAX, esd_comm_world);
  PMPI_Reduce(&lpaths, &maxpaths, 1, MPI_UNSIGNED, MPI_MAX, Unifier, esd_comm_world);
  PMPI_Reduce(&frames, &maxframes, 1, MPI_UNSIGNED, MPI_MAX, Unifier, esd_comm_world);
  PMPI_Allreduce(&unknowns, &esd_unknowns, 1, MPI_UNSIGNED, MPI_SUM, esd_comm_world);
#endif

  if (maxbytes > bsize) {
      esd_summary = 0; /* disable (further) summarisation */
      if (my_rank == Unifier) {
          elg_warning("Definitions lost due to insufficient buffer capacity!");
          elg_warning("ESD_BUFFER_SIZE needs to be at least %d bytes", maxbytes);
	  elg_warning("Unifying... aborted!");
      }
      return 0;
  } else {
      if (my_rank == Unifier)
          elg_warning("Largest definitions buffer %d bytes", maxbytes);
  }

  EsdUnifyData* udata = EsdProc_unify_defs(defbuf, defbytes, my_rank, num_ranks);
  if (esd_tracing)
    EsdProc_write_unify_data(epk_archive_get_name(), udata, my_rank, num_ranks);

  if (my_rank == Unifier) {
    esd_total_paths = epk_vector_size(udata->cpaths);
    elg_warning("%d unique paths (%d max paths, %d max frames, %d unknowns)",
                    esd_total_paths, maxpaths, maxframes, esd_unknowns);
    if (esd_unknowns) elg_warning("Hint: Increase ESD_PATHS to avoid/reduce unknown paths");

    esd_report_paths(udata, esd_unknowns); /* generate paths report */

    if (esd_summary) {
        determine_pathtypes(udata);
        /* open report and write unified definitions */
        if (esd_report_open(NULL)) esd_report_defs(udata, esd_unknowns);
    }
  }

  const EpkIdMap* idmap = esd_unify_get_map(udata, ESD_UNIFY_CPATHMAP);
  const elg_ui4* idmapv = epk_idmap_data(idmap, &esd_idmapsz); /* DENSE */
  esd_idmapv = calloc(esd_idmapsz, sizeof(elg_ui4));
  memcpy(esd_idmapv, idmapv, esd_idmapsz * sizeof(elg_ui4));

  esd_unify_free(udata);

#if (defined (ELG_MPI) || defined(ELG_OMPI))
    /* ensure all processes know number of unique paths */
    PMPI_Bcast(&esd_total_paths, 1, MPI_UNSIGNED, 0, esd_comm_world);
    /* NB: pseudo-barrier since Unifier rank 0 is busy until all ranks are unified */
    /* (prevents interference with subsequent trace flushing, if no summary collation) */
#endif

    if (verb_unify) elg_verbosity(0);

    if (my_rank == Unifier)
        elg_warning("Unifying... done (%0.3fs)", elg_pform_wtime()-start_time);
    return 1;
}

void EsdProc_collate (EsdProc* proc, elg_ui4 my_rank, elg_ui4 num_ranks, elg_ui4 num_thrds)
{
    unsigned t;
    static unsigned Unifier = 0; /* master */
    double start_time = elg_pform_wtime();
    elg_ui4 local_paths = esd_paths_size(thrdv[0]->paths);
    double* sevs = calloc(num_thrds, sizeof(double)); /* sev for proc threads */
    double* sevbuf = calloc(num_ranks*num_thrds, sizeof(double));
    int ready = (sevs && sevbuf);

    char *verb_collate = strstr(epk_get(EPK_VERBOSE),"collate");
    if (verb_collate && elg_rank_match(my_rank, verb_collate+strlen("collate")))
        elg_verbosity(1);

    for (t=0; t<num_thrds; t++) {
        thrdv[t]->paths->rev_idmapv = calloc(esd_total_paths+1, sizeof(elg_ui4));
        if (!thrdv[t]->paths->rev_idmapv) ready=0;
    }

    elg_cntl_msg("Collating report (%d/%d paths)...", local_paths, esd_total_paths);

#if (defined (ELG_MPI) || defined(ELG_OMPI))
    if (my_rank != Unifier) esd_pathtypev = calloc(esd_total_paths, sizeof(elg_ui1));
    ready = (ready && esd_pathtypev);
#endif

    /* verify readiness of all ranks to collate report */
    if (!ready) elg_warning("Failed to allocate collation storage!");
    int allready = ready;
#if (defined (ELG_MPI) || defined(ELG_OMPI))
    PMPI_Allreduce(&ready, &allready, 1, MPI_INT, MPI_SUM, esd_comm_world);
#endif
    if (allready != num_ranks) return; /* no report possible */

#if (defined (ELG_MPI) || defined(ELG_OMPI))
    /* distribute callpath type vector */
    PMPI_Bcast(esd_pathtypev, esd_total_paths, MPI_BYTE, Unifier, esd_comm_world);
#endif

    int gather = (getenv("ESD_GATHER_ALL") != NULL);
    if (my_rank == Unifier) elg_cntl_msg("ESD_GATHER_ALL=%d", gather);

    /* initialise reverse map global->local (for master thread) */
    unsigned localid;
    for (localid = 0; localid < local_paths; localid++) {
        elg_ui4 unifyid = esd_idmapv[localid];
        thrdv[0]->paths->rev_idmapv[unifyid] = localid;
        if (unifyid != localid)
            elg_cntl_msg("cpath idmapv[%d] = %d", localid, unifyid);
    }
    /* initialise reverse map global->local for worker threads */
    for (t=1; t<num_thrds; t++) {
        elg_ui4 local_paths = esd_paths_size(thrdv[t]->paths);
        for (localid = 0; localid < local_paths; localid++) {
            elg_ui4 masterid = thrdv[t]->paths->masterv[localid];
            elg_ui4 unifyid = esd_idmapv[masterid];
            thrdv[t]->paths->rev_idmapv[unifyid] = localid;
            elg_cntl_msg("t%d[%d]->t0[%d]->U[%d]", t, localid, masterid, unifyid);
        }
    }

    unsigned unknown = (esd_unknowns) ? 0 : 1;
    unsigned metid, pathid;
#if defined(MSG_HACK)
    const unsigned MPI_METRICS=13, OMP_METRICS=2;
    const unsigned HACK_METRICS=MPI_METRICS+OMP_METRICS;
#else
    const unsigned HACK_METRICS=0;
#endif
    for (metid = 0; metid < esd_num_metrics+2+HACK_METRICS; metid++) {
        for (pathid = unknown; pathid < esd_total_paths; pathid++) {
#if defined(MSG_HACK)
#if (defined (ELG_MPI) || defined(ELG_OMPI))
            /* no need to gather MPI statistics from non-MPI callpaths */
            if (!gather) switch (metid) {
                case  1: /* coll_syncs */
                case  4: /* coll_c_xch */
                case  5: /* coll_c_src */
                case  6: /* coll_c_dst */
                case  9: /* coll_bsent */
                case 10: /* coll_brcvd */
                    /* skip to next pathid with collective metric data */
                    while ((pathid < esd_total_paths) &&
                           !(esd_pathtypev[pathid]&EPK_MPI_TYPE__COLLECTIVE))
                        pathid++;
                    if (pathid == esd_total_paths) continue;
                    break;
                case  2: /* send_syncs */
                case  7: /* send_comms */
                case 11: /* send_bytes */
                    /* skip to next pathid with send metric data */
                    while ((pathid < esd_total_paths) &&
                           !(esd_pathtypev[pathid]&EPK_MPI_TYPE__SEND))
                        pathid++;
                    if (pathid == esd_total_paths) continue;
                    break;
                case  3: /* recv_syncs */
                case  8: /* recv_comms */
                case 12: /* recv_bytes */
                    /* skip to next pathid with recv metric data */
                    while ((pathid < esd_total_paths) &&
                           !(esd_pathtypev[pathid]&EPK_MPI_TYPE__RECV))
                        pathid++;
                    if (pathid == esd_total_paths) continue;
                    break;
                case 13: /* file_bytes */
                    /* skip to next pathid with file metric data */
                    while ((pathid < esd_total_paths) &&
                           !(esd_pathtypev[pathid]&PATH_TYPE__FILE))
                        pathid++;
                    if (pathid == esd_total_paths) continue;
                   elg_cntl_msg("Collating bytes for MPI File path %d", pathid);
                    break;
                default:
                    break;
            }
#else
            /* no need to gather MPI statistics from non-MPI programs */
            if (!gather && ((metid >= 1) && (metid <= MPI_METRICS))) continue;
#endif
            /* XXXX OMP_HACK */
            if ((metid == 14) || (metid == 15)) {
                /* skip to next OMP pathid */
                while ((pathid < esd_total_paths) &&
                       !(esd_pathtypev[pathid]&PATH_TYPE__OMP))
                    pathid++;
                if (pathid == esd_total_paths) continue;
                elg_cntl_msg("Collating time for OMP path %d", pathid);
            }
#endif
            for (t=0; t<num_thrds; t++) { /* for each thread on process */
                /* determine local path identifier */
                unsigned lpathid = thrdv[t]->paths->rev_idmapv[pathid];
                /* extract this path's severity value */
                if (metid == 0)
                    sevs[t] = esd_path_visits(thrdv[t]->paths, lpathid);
                else if (metid == 1)
#if defined(MSG_HACK)
                /* hack for message statistics: XXX check range above XXX */
                    sevs[t] = esd_path_coll_syncs(thrdv[t]->paths, lpathid);
                else if (metid == 2)
                    sevs[t] = esd_path_send_syncs(thrdv[t]->paths, lpathid);
                else if (metid == 3)
                    sevs[t] = esd_path_recv_syncs(thrdv[t]->paths, lpathid);
                else if (metid == 4)
                    sevs[t] = esd_path_coll_c_xch(thrdv[t]->paths, lpathid);
                else if (metid == 5)
                    sevs[t] = esd_path_coll_c_src(thrdv[t]->paths, lpathid);
                else if (metid == 6)
                    sevs[t] = esd_path_coll_c_dst(thrdv[t]->paths, lpathid);
                else if (metid == 7)
                    sevs[t] = esd_path_send_comms(thrdv[t]->paths, lpathid);
                else if (metid == 8)
                    sevs[t] = esd_path_recv_comms(thrdv[t]->paths, lpathid);
                else if (metid == 9)
                    sevs[t] = esd_path_coll_bsent(thrdv[t]->paths, lpathid);
                else if (metid == 10)
                    sevs[t] = esd_path_coll_brcvd(thrdv[t]->paths, lpathid);
                else if (metid == 11)
                    sevs[t] = esd_path_send_bytes(thrdv[t]->paths, lpathid);
                else if (metid == 12)
                    sevs[t] = esd_path_recv_bytes(thrdv[t]->paths, lpathid);
                else if (metid == 13)
                    sevs[t] = esd_path_file_bytes(thrdv[t]->paths, lpathid);
                else if (metid == 14)
/* XXXX OMP_HACK */
                    sevs[t] = esd_path_tfork_time(thrdv[t]->paths, lpathid);
                else if (metid == 15)
                    sevs[t] = esd_path_tmgmt_time(thrdv[t]->paths, lpathid);
                else if (metid == (HACK_METRICS+1))
#endif
                    sevs[t] = esd_path_timing(thrdv[t]->paths, lpathid);
                else
                    sevs[t] = esd_path_metric(thrdv[t]->paths, lpathid, metid-2-HACK_METRICS);
                sevbuf[t] = sevs[t];
            }

#if (defined (ELG_MPI) || defined(ELG_OMPI))
            elg_cntl_msg("Gather met=%d path=%d", metid, pathid);
            PMPI_Gather(sevs, num_thrds, MPI_DOUBLE,
                        sevbuf, num_thrds, MPI_DOUBLE, Unifier, esd_comm_world);
            PMPI_Barrier(esd_comm_world); /* hold back network flood */
#endif

            if (my_rank == Unifier) { /* add severities to report */
#if DEBUG
                elg_ui4 total = 0;
                if (metid == 0) {
                    for (unsigned r=0; r<num_ranks; r++) {
                        elg_ui4 visits = sevbuf[r];
                        total += visits;
                        printf("P%03u:visits[%u]=%u\n", pathid, r, visits);
                    }
/*                  printf("P%03u:visits=%u\n", pathid, total);*/
                    esd_report_visits(pathid-unknown, total);
                }
#endif
                esd_report_sev_threads(metid, pathid-unknown, sevbuf);
            }
        } /* pathid */
    } /* metid */

#if (defined (ELG_MPI) || defined(ELG_OMPI))
    PMPI_Barrier(esd_comm_world); /* XXXX temp sync */
#endif

    if (verb_collate) elg_verbosity(0);

    if (my_rank == Unifier) {
        esd_report_close();
        elg_warning("Collating... done (%0.3fs)", elg_pform_wtime()-start_time);
    }
}
