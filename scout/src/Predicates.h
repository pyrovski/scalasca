/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <string>

#include <Region.h>


#ifndef SCOUT_PREDICATES_H
#define SCOUT_PREDICATES_H


namespace scout
{

/*
 *---------------------------------------------------------------------------
 *
 * Predicate functions
 *
 *---------------------------------------------------------------------------
 */

/* General */
bool is_overhead(const pearl::Region* region);

/* MPI related */
bool is_mpi_api(const pearl::Region* region);
bool is_mpi_comm(const pearl::Region* region);
bool is_mpi_collcomm(const pearl::Region* region);
bool is_mpi_p2p(const pearl::Region* region);
bool is_mpi_io(const pearl::Region* region);
bool is_mpi_init(const pearl::Region* region);
bool is_mpi_finalize(const pearl::Region* region);
bool is_mpi_barrier(const pearl::Region* region);
bool is_mpi_testx(const pearl::Region* region);
bool is_mpi_waitx(const pearl::Region* region);
bool is_mpi_wait_single(const pearl::Region* region);
bool is_mpi_wait_multi(const pearl::Region* region);
bool is_mpi_block_send(const pearl::Region* region);
bool is_mpi_12n(const pearl::Region* region);
bool is_mpi_n21(const pearl::Region* region);
bool is_mpi_n2n(const pearl::Region* region);
bool is_mpi_scan(const pearl::Region* region);
bool is_mpi_rma_start(const pearl::Region* region);
bool is_mpi_rma_complete(const pearl::Region* region);
bool is_mpi_rma_post(const pearl::Region* region);
bool is_mpi_rma_wait(const pearl::Region* region);
bool is_mpi_rma_test(const pearl::Region* region);
bool is_mpi_rma_put(const pearl::Region* region);
bool is_mpi_rma_get(const pearl::Region* region);
bool is_mpi_rma_fence(const pearl::Region* region);

/* OpenMP related */
bool is_omp_api(const pearl::Region* region);
bool is_omp_parallel(const pearl::Region* region);
bool is_omp_ebarrier(const pearl::Region* region);
bool is_omp_ibarrier(const pearl::Region* region);


//--- Inline function implementation ----------------------------------------


inline bool is_overhead(const pearl::Region* region)
{
  return (region->get_class() & pearl::Region::CLASS_INTERNAL);
}


inline bool is_mpi_api(const pearl::Region* region)
{
  return (region->get_class() & pearl::Region::CLASS_MPI);
}


inline bool is_mpi_comm(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::CAT_MPI_P2P;

  return (region->get_class() & mask) > pearl::Region::CLASS_MPI;
}


inline bool is_mpi_collcomm(const pearl::Region* region)
{
  uint32_t mask  = pearl::Region::CLASS_MPI
                   | pearl::Region::CAT_MPI_COLLECTIVE
                   | pearl::Region::TYPE_MPI_ONE_TO_N
                   | pearl::Region::TYPE_MPI_N_TO_ONE
                   | pearl::Region::TYPE_MPI_N_TO_N
                   | pearl::Region::TYPE_MPI_PARTIAL;
  uint32_t value = pearl::Region::CLASS_MPI
                   | pearl::Region::CAT_MPI_COLLECTIVE;

  return (region->get_class() & mask) > value;
}


inline bool is_mpi_p2p(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI | pearl::Region::CAT_MPI_P2P;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_io(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI | pearl::Region::CAT_MPI_IO;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_init(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_SETUP
                  | pearl::Region::TYPE_MPI_INIT;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_finalize(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_SETUP
                  | pearl::Region::TYPE_MPI_FINALIZE;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_barrier(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::TYPE_MPI_BARRIER;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_testx(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_P2P
                  | pearl::Region::TYPE_MPI_TEST;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_waitx(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_P2P
                  | pearl::Region::TYPE_MPI_WAIT;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_wait_single(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_P2P
                  | pearl::Region::TYPE_MPI_WAIT
                  | pearl::Region::MODE_MPI_SINGLE;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_wait_multi(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_P2P
                  | pearl::Region::TYPE_MPI_WAIT
                  | pearl::Region::MODE_MPI_MANY;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_block_send(const pearl::Region* region)
{
  uint32_t mask  = pearl::Region::CLASS_MPI
                   | pearl::Region::CAT_MPI_P2P
                   | pearl::Region::TYPE_MPI_SEND
                   | pearl::Region::MODE_MPI_STANDARD
                   | pearl::Region::MODE_MPI_SYNCHRONOUS;
  uint32_t value = region->get_class() ^ mask;

  return (value == pearl::Region::MODE_MPI_STANDARD) ||
         (value == pearl::Region::MODE_MPI_SYNCHRONOUS);
}


inline bool is_mpi_12n(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::TYPE_MPI_ONE_TO_N;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_n21(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::TYPE_MPI_N_TO_ONE;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_n2n(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::TYPE_MPI_N_TO_N;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_scan(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_COLLECTIVE
                  | pearl::Region::TYPE_MPI_PARTIAL;

  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_start(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_GATS
                  | pearl::Region::MODE_RMA_START;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_complete(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_GATS
                  | pearl::Region::MODE_RMA_COMPLETE;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_post(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_GATS
                  | pearl::Region::MODE_RMA_POST;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_wait(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_GATS
                  | pearl::Region::MODE_RMA_WAIT;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_test(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_GATS
                  | pearl::Region::MODE_RMA_TEST;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_put(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_COMM
                  | pearl::Region::MODE_RMA_PUT;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_get(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_COMM
                  | pearl::Region::MODE_RMA_GET;
  return (region->get_class() & mask) == mask;
}


inline bool is_mpi_rma_fence(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_RMA
                  | pearl::Region::TYPE_MPI_RMA_COLL
                  | pearl::Region::MODE_RMA_FENCE;
  return (region->get_class() & mask) == mask;
}


inline bool is_omp_api(const pearl::Region* region)
{
  return (region->get_class() & pearl::Region::CLASS_OMP);
}


inline bool is_omp_parallel(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_OMP
                  | pearl::Region::CAT_OMP_PARALLEL;

  return (region->get_class() & mask) == mask;
}

inline bool is_omp_ibarrier(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_OMP
                  | pearl::Region::CAT_OMP_SYNC
                  | pearl::Region::TYPE_OMP_BARRIER
                  | pearl::Region::MODE_OMP_IMPLICIT;
  return (region->get_class() & mask) == mask;
}

inline bool is_omp_ebarrier(const pearl::Region* region)
{
  uint32_t mask = pearl::Region::CLASS_OMP
                  | pearl::Region::CAT_OMP_SYNC
                  | pearl::Region::TYPE_OMP_BARRIER
                  | pearl::Region::MODE_OMP_EXPLICIT;

  return (region->get_class() & mask) == mask;
}


}   /* namespace scout */


#endif   /* !SCOUT_PREDICATES_H */
