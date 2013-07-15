/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_fmpiwrap_topo.c
 *
 * @brief Fortran interface wrappers for topologies
 */

#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_fmpiwrap.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

/* uppercase defines */
/** All uppercase name of MPI_Cart_coords */
#define MPI_Cart_coords_U MPI_CART_COORDS
/** All uppercase name of MPI_Cart_create */
#define MPI_Cart_create_U MPI_CART_CREATE
/** All uppercase name of MPI_Cart_get */
#define MPI_Cart_get_U MPI_CART_GET
/** All uppercase name of MPI_Cart_map */
#define MPI_Cart_map_U MPI_CART_MAP
/** All uppercase name of MPI_Cart_rank */
#define MPI_Cart_rank_U MPI_CART_RANK
/** All uppercase name of MPI_Cart_shift */
#define MPI_Cart_shift_U MPI_CART_SHIFT
/** All uppercase name of MPI_Cart_sub */
#define MPI_Cart_sub_U MPI_CART_SUB
/** All uppercase name of MPI_Cartdim_get */
#define MPI_Cartdim_get_U MPI_CARTDIM_GET
/** All uppercase name of MPI_Dims_create */
#define MPI_Dims_create_U MPI_DIMS_CREATE
/** All uppercase name of MPI_Dist_graph_create */
#define MPI_Dist_graph_create_U MPI_DIST_GRAPH_CREATE
/** All uppercase name of MPI_Dist_graph_create_adjacent */
#define MPI_Dist_graph_create_adjacent_U MPI_DIST_GRAPH_CREATE_ADJACENT
/** All uppercase name of MPI_Dist_graph_neighbors */
#define MPI_Dist_graph_neighbors_U MPI_DIST_GRAPH_NEIGHBORS
/** All uppercase name of MPI_Dist_graph_neighbors_count */
#define MPI_Dist_graph_neighbors_count_U MPI_DIST_GRAPH_NEIGHBORS_COUNT
/** All uppercase name of MPI_Graph_create */
#define MPI_Graph_create_U MPI_GRAPH_CREATE
/** All uppercase name of MPI_Graph_get */
#define MPI_Graph_get_U MPI_GRAPH_GET
/** All uppercase name of MPI_Graph_map */
#define MPI_Graph_map_U MPI_GRAPH_MAP
/** All uppercase name of MPI_Graph_neighbors */
#define MPI_Graph_neighbors_U MPI_GRAPH_NEIGHBORS
/** All uppercase name of MPI_Graph_neighbors_count */
#define MPI_Graph_neighbors_count_U MPI_GRAPH_NEIGHBORS_COUNT
/** All uppercase name of MPI_Graphdims_get */
#define MPI_Graphdims_get_U MPI_GRAPHDIMS_GET
/** All uppercase name of MPI_Topo_test */
#define MPI_Topo_test_U MPI_TOPO_TEST

/* lowercase defines */
/** All lowercase name of MPI_Cart_coords */
#define MPI_Cart_coords_L mpi_cart_coords
/** All lowercase name of MPI_Cart_create */
#define MPI_Cart_create_L mpi_cart_create
/** All lowercase name of MPI_Cart_get */
#define MPI_Cart_get_L mpi_cart_get
/** All lowercase name of MPI_Cart_map */
#define MPI_Cart_map_L mpi_cart_map
/** All lowercase name of MPI_Cart_rank */
#define MPI_Cart_rank_L mpi_cart_rank
/** All lowercase name of MPI_Cart_shift */
#define MPI_Cart_shift_L mpi_cart_shift
/** All lowercase name of MPI_Cart_sub */
#define MPI_Cart_sub_L mpi_cart_sub
/** All lowercase name of MPI_Cartdim_get */
#define MPI_Cartdim_get_L mpi_cartdim_get
/** All lowercase name of MPI_Dims_create */
#define MPI_Dims_create_L mpi_dims_create
/** All lowercase name of MPI_Dist_graph_create */
#define MPI_Dist_graph_create_L mpi_dist_graph_create
/** All lowercase name of MPI_Dist_graph_create_adjacent */
#define MPI_Dist_graph_create_adjacent_L mpi_dist_graph_create_adjacent
/** All lowercase name of MPI_Dist_graph_neighbors */
#define MPI_Dist_graph_neighbors_L mpi_dist_graph_neighbors
/** All lowercase name of MPI_Dist_graph_neighbors_count */
#define MPI_Dist_graph_neighbors_count_L mpi_dist_graph_neighbors_count
/** All lowercase name of MPI_Graph_create */
#define MPI_Graph_create_L mpi_graph_create
/** All lowercase name of MPI_Graph_get */
#define MPI_Graph_get_L mpi_graph_get
/** All lowercase name of MPI_Graph_map */
#define MPI_Graph_map_L mpi_graph_map
/** All lowercase name of MPI_Graph_neighbors */
#define MPI_Graph_neighbors_L mpi_graph_neighbors
/** All lowercase name of MPI_Graph_neighbors_count */
#define MPI_Graph_neighbors_count_L mpi_graph_neighbors_count
/** All lowercase name of MPI_Graphdims_get */
#define MPI_Graphdims_get_L mpi_graphdims_get
/** All lowercase name of MPI_Topo_test */
#define MPI_Topo_test_L mpi_topo_test

/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined(HAS_MPI_CART_COORDS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_coords)(MPI_Comm* comm,
                           int*      rank,
                           int*      maxdims,
                           int*      coords,
                           int*      ierr)
{
  *ierr = MPI_Cart_coords(*comm, *rank, *maxdims, coords);
}

#endif
#if defined(HAS_MPI_CART_CREATE)
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_create)(MPI_Comm* comm_old,
                           int*      ndims,
                           int*      dims,
                           int*      periods,
                           int*      reorder,
                           MPI_Comm* comm_cart,
                           int*      ierr)
{
  *ierr = MPI_Cart_create(*comm_old,
                          *ndims,
                          dims,
                          periods,
                          *reorder,
                          comm_cart);
}

#endif
#if defined(HAS_MPI_CART_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_get)(MPI_Comm* comm,
                        int*      maxdims,
                        int*      dims,
                        int*      periods,
                        int*      coords,
                        int*      ierr)
{
  *ierr = MPI_Cart_get(*comm, *maxdims, dims, periods, coords);
}

#endif
#if defined(HAS_MPI_CART_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_map)(MPI_Comm* comm,
                        int*      ndims,
                        int*      dims,
                        int*      periods,
                        int*      newrank,
                        int*      ierr)
{
  *ierr = MPI_Cart_map(*comm, *ndims, dims, periods, newrank);
}

#endif
#if defined(HAS_MPI_CART_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_rank)(MPI_Comm* comm,
                         int*      coords,
                         int*      rank,
                         int*      ierr)
{
  *ierr = MPI_Cart_rank(*comm, coords, rank);
}

#endif
#if defined(HAS_MPI_CART_SHIFT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_shift)(MPI_Comm* comm,
                          int*      direction,
                          int*      disp,
                          int*      rank_source,
                          int*      rank_dest,
                          int*      ierr)
{
  *ierr = MPI_Cart_shift(*comm, *direction, *disp, rank_source, rank_dest);
}

#endif
#if defined(HAS_MPI_CART_SUB)
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_sub)(MPI_Comm* comm,
                        int*      remain_dims,
                        MPI_Comm* newcomm,
                        int*      ierr)
{
  *ierr = MPI_Cart_sub(*comm, remain_dims, newcomm);
}

#endif
#if defined(HAS_MPI_CARTDIM_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cartdim_get)(MPI_Comm* comm,
                           int*      ndims,
                           int*      ierr)
{
  *ierr = MPI_Cartdim_get(*comm, ndims);
}

#endif
#if defined(HAS_MPI_DIMS_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Dims_create)(int* nnodes,
                           int* ndims,
                           int* dims,
                           int* ierr)
{
  *ierr = MPI_Dims_create(*nnodes, *ndims, dims);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_CREATE) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_create)(MPI_Comm* comm_old,
                                 int*      n,
                                 int       sources[],
                                 int       degrees[],
                                 int       destinations[],
                                 int       weights[],
                                 MPI_Info* info,
                                 int*      reorder,
                                 MPI_Comm* newcomm,
                                 int*      ierr)
{
  #if defined(HAS_MPI_UNWEIGHTED)
  if (weights == epk_mpif_unweighted)
  {
    weights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_create(*comm_old,
                                *n,
                                sources,
                                degrees,
                                destinations,
                                weights,
                                *info,
                                *reorder,
                                newcomm);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_CREATE_ADJACENT) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_create_adjacent)(MPI_Comm* comm_old,
                                          int*      indegree,
                                          int       sources[],
                                          int       sourceweights[],
                                          int*      outdegree,
                                          int       destinations[],
                                          int       destweights[],
                                          MPI_Info* info,
                                          int*      reorder,
                                          MPI_Comm* newcomm,
                                          int*      ierr)
{
  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == epk_mpif_unweighted)
  {
    sourceweights = MPI_UNWEIGHTED;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == epk_mpif_unweighted)
  {
    destweights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_create_adjacent(*comm_old,
                                         *indegree,
                                         sources,
                                         sourceweights,
                                         *outdegree,
                                         destinations,
                                         destweights,
                                         *info,
                                         *reorder,
                                         newcomm);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_neighbors)(MPI_Comm* comm,
                                    int*      maxindegree,
                                    int       sources[],
                                    int       sourceweights[],
                                    int*      maxoutdegree,
                                    int       destinations[],
                                    int       destweights[],
                                    int*      ierr)
{
  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == epk_mpif_unweighted)
  {
    sourceweights = MPI_UNWEIGHTED;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == epk_mpif_unweighted)
  {
    destweights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_neighbors(*comm,
                                   *maxindegree,
                                   sources,
                                   sourceweights,
                                   *maxoutdegree,
                                   destinations,
                                   destweights);

  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == MPI_UNWEIGHTED)
  {
    sourceweights = epk_mpif_unweighted;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == MPI_UNWEIGHTED)
  {
    destweights = epk_mpif_unweighted;
  }
  #endif
} /* FSUB(MPI_Dist_graph_neighbors) */

#endif
#if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS_COUNT) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_neighbors_count)(MPI_Comm* comm,
                                          int*      indegree,
                                          int*      outdegree,
                                          int*      weighted,
                                          int*      ierr)
{
  *ierr = MPI_Dist_graph_neighbors_count(*comm,
                                         indegree,
                                         outdegree,
                                         weighted);
}

#endif
#if defined(HAS_MPI_GRAPH_CREATE)
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_create)(MPI_Comm* comm_old,
                            int*      nnodes,
                            int*      index,
                            int*      edges,
                            int*      reorder,
                            MPI_Comm* newcomm,
                            int*      ierr)
{
  *ierr = MPI_Graph_create(*comm_old,
                           *nnodes,
                           index,
                           edges,
                           *reorder,
                           newcomm);
}

#endif
#if defined(HAS_MPI_GRAPH_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_get)(MPI_Comm* comm,
                         int*      maxindex,
                         int*      maxedges,
                         int*      index,
                         int*      edges,
                         int*      ierr)
{
  *ierr = MPI_Graph_get(*comm, *maxindex, *maxedges, index, edges);
}

#endif
#if defined(HAS_MPI_GRAPH_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_map)(MPI_Comm* comm,
                         int*      nnodes,
                         int*      index,
                         int*      edges,
                         int*      newrank,
                         int*      ierr)
{
  *ierr = MPI_Graph_map(*comm, *nnodes, index, edges, newrank);
}

#endif
#if defined(HAS_MPI_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_neighbors)(MPI_Comm* comm,
                               int*      rank,
                               int*      maxneighbors,
                               int*      neighbors,
                               int*      ierr)
{
  *ierr = MPI_Graph_neighbors(*comm, *rank, *maxneighbors, neighbors);
}

#endif
#if defined(HAS_MPI_GRAPH_NEIGHBORS_COUNT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_neighbors_count)(MPI_Comm* comm,
                                     int*      rank,
                                     int*      nneighbors,
                                     int*      ierr)
{
  *ierr = MPI_Graph_neighbors_count(*comm, *rank, nneighbors);
}

#endif
#if defined(HAS_MPI_GRAPHDIMS_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graphdims_get)(MPI_Comm* comm,
                             int*      nnodes,
                             int*      nedges,
                             int*      ierr)
{
  *ierr = MPI_Graphdims_get(*comm, nnodes, nedges);
}

#endif
#if defined(HAS_MPI_TOPO_TEST) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Topo_test)(MPI_Comm* comm,
                         int*      status,
                         int*      ierr)
{
  *ierr = MPI_Topo_test(*comm, status);
}

#endif

#else /* !NEED_F2C_CONV */

#if defined(HAS_MPI_CART_COORDS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_coords)(MPI_Fint* comm,
                           MPI_Fint* rank,
                           MPI_Fint* maxdims,
                           MPI_Fint* coords,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Cart_coords(PMPI_Comm_f2c(*comm), *rank, *maxdims, coords);
}

#endif
#if defined(HAS_MPI_CART_CREATE)
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_create)(MPI_Fint* comm_old,
                           MPI_Fint* ndims,
                           MPI_Fint* dims,
                           MPI_Fint* periods,
                           MPI_Fint* reorder,
                           MPI_Fint* comm_cart,
                           MPI_Fint* ierr)
{
  MPI_Comm c_comm_cart;

  *ierr = MPI_Cart_create(PMPI_Comm_f2c(
                            *comm_old), *ndims, dims, periods, *reorder,
                          &c_comm_cart);

  *comm_cart = PMPI_Comm_c2f(c_comm_cart);
}

#endif
#if defined(HAS_MPI_CART_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_get)(MPI_Fint* comm,
                        MPI_Fint* maxdims,
                        MPI_Fint* dims,
                        MPI_Fint* periods,
                        MPI_Fint* coords,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Cart_get(PMPI_Comm_f2c(
                         *comm), *maxdims, dims, periods, coords);
}

#endif
#if defined(HAS_MPI_CART_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_map)(MPI_Fint* comm,
                        MPI_Fint* ndims,
                        MPI_Fint* dims,
                        MPI_Fint* periods,
                        MPI_Fint* newrank,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Cart_map(PMPI_Comm_f2c(
                         *comm), *ndims, dims, periods, newrank);
}

#endif
#if defined(HAS_MPI_CART_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_rank)(MPI_Fint* comm,
                         MPI_Fint* coords,
                         MPI_Fint* rank,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Cart_rank(PMPI_Comm_f2c(*comm), coords, rank);
}

#endif
#if defined(HAS_MPI_CART_SHIFT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_shift)(MPI_Fint* comm,
                          MPI_Fint* direction,
                          MPI_Fint* disp,
                          MPI_Fint* rank_source,
                          MPI_Fint* rank_dest,
                          MPI_Fint* ierr)
{
  *ierr = MPI_Cart_shift(PMPI_Comm_f2c(
                           *comm), *direction, *disp, rank_source,
                         rank_dest);
}

#endif
#if defined(HAS_MPI_CART_SUB)
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cart_sub)(MPI_Fint* comm,
                        MPI_Fint* remain_dims,
                        MPI_Fint* newcomm,
                        MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Cart_sub(PMPI_Comm_f2c(*comm), remain_dims, &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_CARTDIM_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Cartdim_get)(MPI_Fint* comm,
                           MPI_Fint* ndims,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Cartdim_get(PMPI_Comm_f2c(*comm), ndims);
}

#endif
#if defined(HAS_MPI_DIMS_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Dims_create)(MPI_Fint* nnodes,
                           MPI_Fint* ndims,
                           MPI_Fint* dims,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Dims_create(*nnodes, *ndims, dims);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_CREATE) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_create)(MPI_Fint* comm_old,
                                 MPI_Fint* n,
                                 MPI_Fint* sources,
                                 MPI_Fint* degrees,
                                 MPI_Fint* destinations,
                                 MPI_Fint* weights,
                                 MPI_Fint* info,
                                 MPI_Fint* reorder,
                                 MPI_Fint* newcomm,
                                 MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  #if defined(HAS_MPI_UNWEIGHTED)
  if (weights == epk_mpif_unweighted)
  {
    weights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_create(PMPI_Comm_f2c(
                                  *comm_old), *n, sources, degrees,
                                destinations, weights,
                                PMPI_Info_f2c(*info), *reorder, &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_CREATE_ADJACENT) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_create_adjacent)(MPI_Fint* comm_old,
                                          MPI_Fint* indegree,
                                          MPI_Fint* sources,
                                          MPI_Fint* sourceweights,
                                          MPI_Fint* outdegree,
                                          MPI_Fint* destinations,
                                          MPI_Fint* destweights,
                                          MPI_Fint* info,
                                          MPI_Fint* reorder,
                                          MPI_Fint* newcomm,
                                          MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == epk_mpif_unweighted)
  {
    sourceweights = MPI_UNWEIGHTED;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == epk_mpif_unweighted)
  {
    destweights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_create_adjacent(PMPI_Comm_f2c(
                                           *comm_old), *indegree, sources,
                                         sourceweights, *outdegree,
                                         destinations,
                                         destweights, PMPI_Info_f2c(
                                           *info), *reorder, &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_neighbors)(MPI_Fint* comm,
                                    MPI_Fint* maxindegree,
                                    MPI_Fint* sources,
                                    MPI_Fint* sourceweights,
                                    MPI_Fint* maxoutdegree,
                                    MPI_Fint* destinations,
                                    MPI_Fint* destweights,
                                    MPI_Fint* ierr)
{
  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == epk_mpif_unweighted)
  {
    sourceweights = MPI_UNWEIGHTED;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == epk_mpif_unweighted)
  {
    destweights = MPI_UNWEIGHTED;
  }
  #endif

  *ierr = MPI_Dist_graph_neighbors(PMPI_Comm_f2c(
                                     *comm), *maxindegree, sources,
                                   sourceweights, *maxoutdegree,
                                   destinations,
                                   destweights);

  #if defined(HAS_MPI_UNWEIGHTED)
  if (sourceweights == MPI_UNWEIGHTED)
  {
    sourceweights = epk_mpif_unweighted;
  }
  #endif
  #if defined(HAS_MPI_UNWEIGHTED)
  if (destweights == MPI_UNWEIGHTED)
  {
    destweights = epk_mpif_unweighted;
  }
  #endif
} /* FSUB(MPI_Dist_graph_neighbors) */

#endif
#if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS_COUNT) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup topo
 */
void FSUB(MPI_Dist_graph_neighbors_count)(MPI_Fint* comm,
                                          MPI_Fint* indegree,
                                          MPI_Fint* outdegree,
                                          MPI_Fint* weighted,
                                          MPI_Fint* ierr)
{
  *ierr = MPI_Dist_graph_neighbors_count(PMPI_Comm_f2c(
                                           *comm), indegree, outdegree,
                                         weighted);
}

#endif
#if defined(HAS_MPI_GRAPH_CREATE)
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_create)(MPI_Fint* comm_old,
                            MPI_Fint* nnodes,
                            MPI_Fint* index,
                            MPI_Fint* edges,
                            MPI_Fint* reorder,
                            MPI_Fint* newcomm,
                            MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Graph_create(PMPI_Comm_f2c(
                             *comm_old), *nnodes, index, edges, *reorder,
                           &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_GRAPH_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_get)(MPI_Fint* comm,
                         MPI_Fint* maxindex,
                         MPI_Fint* maxedges,
                         MPI_Fint* index,
                         MPI_Fint* edges,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Graph_get(PMPI_Comm_f2c(
                          *comm), *maxindex, *maxedges, index, edges);
}

#endif
#if defined(HAS_MPI_GRAPH_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_map)(MPI_Fint* comm,
                         MPI_Fint* nnodes,
                         MPI_Fint* index,
                         MPI_Fint* edges,
                         MPI_Fint* newrank,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Graph_map(PMPI_Comm_f2c(
                          *comm), *nnodes, index, edges, newrank);
}

#endif
#if defined(HAS_MPI_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_neighbors)(MPI_Fint* comm,
                               MPI_Fint* rank,
                               MPI_Fint* maxneighbors,
                               MPI_Fint* neighbors,
                               MPI_Fint* ierr)
{
  *ierr = MPI_Graph_neighbors(PMPI_Comm_f2c(
                                *comm), *rank, *maxneighbors, neighbors);
}

#endif
#if defined(HAS_MPI_GRAPH_NEIGHBORS_COUNT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graph_neighbors_count)(MPI_Fint* comm,
                                     MPI_Fint* rank,
                                     MPI_Fint* nneighbors,
                                     MPI_Fint* ierr)
{
  *ierr = MPI_Graph_neighbors_count(PMPI_Comm_f2c(
                                      *comm), *rank, nneighbors);
}

#endif
#if defined(HAS_MPI_GRAPHDIMS_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Graphdims_get)(MPI_Fint* comm,
                             MPI_Fint* nnodes,
                             MPI_Fint* nedges,
                             MPI_Fint* ierr)
{
  *ierr = MPI_Graphdims_get(PMPI_Comm_f2c(*comm), nnodes, nedges);
}

#endif
#if defined(HAS_MPI_TOPO_TEST) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup topo
 */
void FSUB(MPI_Topo_test)(MPI_Fint* comm,
                         MPI_Fint* status,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Topo_test(PMPI_Comm_f2c(*comm), status);
}

#endif

#endif

/**
 * @}
 */
