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

/**
* \file Predicates.h 
* \brief Provides a set of testers for different types of regions.
*/ 
#ifndef CUBE_PREDICATES_H
#define CUBE_PREDICATES_H


#include <string>


namespace cube
{

class Region;


std::string lowercase(const std::string& str);

bool is_epk(const Region* region);
bool is_usr(const Region* region);
bool is_omp(const Region* region);
bool is_mpi(const Region* region);

bool is_omp_api(const std::string& region);
bool is_omp_flush(const std::string& region);
bool is_omp_atomic(const std::string& region);
bool is_omp_ordered(const std::string& region);
bool is_omp_critical(const std::string& region);
bool is_omp_ebarrier(const std::string& region);
bool is_omp_ibarrier(const std::string& region);
bool is_omp_lock_api(const std::string& region);
bool is_omp_parallel(const std::string& region);

bool is_mpi_api(const std::string& region);
bool is_mpi_coll(const std::string& region);
bool is_mpi_p2p(const std::string& region);
bool is_mpi_rma(const std::string& region);
bool is_mpi_io(const std::string& region);
bool is_mpi_io_read(const std::string& region);
bool is_mpi_io_write(const std::string& region);
bool is_mpi_coll_io(const std::string& region);
bool is_mpi_init(const std::string& region);
bool is_mpi_sync(const std::string& region);
bool is_mpi_sync_coll(const std::string& region);
bool is_mpi_sync_rma(const std::string& region);
bool is_mpi_sync_rma_active(const std::string& region);
bool is_mpi_sync_rma_passive(const std::string& region);

bool is_mpi_sync_rma_fence(const std::string& region);
bool is_mpi_sync_rma_gats_access(const std::string& region);
bool is_mpi_sync_rma_gats_exposure(const std::string& region);

bool is_mpi_comm_rma_put(const std::string& region);
bool is_mpi_comm_rma_get(const std::string& region);

}   /* namespace cube */


#endif   /* CUBE_PREDICATES_H */
