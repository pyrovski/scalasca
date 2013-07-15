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


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "esd_unify.h"
#include "elg_error.h"
#include "platform/elg_pform.h"
#include "elg_rw.h"
#include "epk_archive.h"


#define CHUNKSIZE (4 * 1024 * 1024)


/*--- Global variables ----------------------------------------------------*/

MPI_Comm esd_comm_world;
int      esd_tracing = 1;


/*--- Function prototypes -------------------------------------------------*/

int  main(int argc, char** argv);

EsdUnifyData* EsdProc_unify_defs(const buffer_t defBuffer,
                                       size_t   defBytes,
                                       int      myRank,
                                       int      numRanks);
void EsdProc_write_unify_data(const char*         dirname,
                                    EsdUnifyData* unifyData,
                                    int           myRank,
                                    int           numRanks);

void readDefs(const char* dirname, int rank, buffer_t* buffer, size_t* bytes);
size_t fsize(FILE* fp);


/*--- Implementation ------------------------------------------------------*/

int main(int argc, char** argv)
{
  int myRank;
  int numRanks;

  buffer_t defBuffer;
  size_t   defBytes;

  EsdUnifyData* unifyData;


  /* Initialize MPI & EPIK error reporting */
  MPI_Init(&argc, &argv);
  MPI_Comm_dup(MPI_COMM_WORLD, &esd_comm_world);
  MPI_Comm_rank(esd_comm_world, &myRank);
  MPI_Comm_size(esd_comm_world, &numRanks);
  elg_error_pid(myRank);

  /* Validate number of arguments */
  if (argc != 2)
    elg_error_msg("Usage: <mpi_launch_cmd> elg_unify.mpi <dirname>");

  /* Read local definitions from file */
  readDefs(argv[1], myRank, &defBuffer, &defBytes);
  MPI_Barrier(esd_comm_world);

  /* Unify and create mappings */
  unifyData = EsdProc_unify_defs(defBuffer, defBytes, myRank, numRanks);
  free(defBuffer);
  if (esd_tracing)
    EsdProc_write_unify_data(argv[1], unifyData, myRank, numRanks);

  /* Clean up */
  esd_unify_free(unifyData);
  MPI_Comm_free(&esd_comm_world);
  MPI_Finalize();

  return EXIT_SUCCESS;
}


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


void readDefs(const char* dirname, int rank, buffer_t* buffer, size_t* bytes)
{
  char* esdName;
  FILE* esdFile;

  /* Open local definition file */
  esdName = epk_archive_rankname(EPK_TYPE_ESD, dirname, rank);
  esdFile = fopen(esdName, "rb");
  if (!esdFile)
    elg_error_msg("Could not open local definition file \"%s\"", esdName);

  /* Determine file size and allocate buffer */
  *bytes  = fsize(esdFile);
  *buffer = (buffer_t)malloc(*bytes);
  if (!*buffer)
    elg_error_msg("Failed to malloc local definition buffer");

  /* Read file contents */
  if (fread(*buffer, *bytes, 1, esdFile) != 1)
    elg_error_msg("Error reading local definition file \"%s\"", esdName);

  /* Clean up */
  fclose(esdFile);
  free(esdName);
}


size_t fsize(FILE* fp)
{
  long current;
  long result;

  /* Determine file size */
  current = ftell(fp);
  fseek(fp, 0, SEEK_END);
  result = ftell(fp);
  fseek(fp, current, SEEK_SET);

  return result;
}
