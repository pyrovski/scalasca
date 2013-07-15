/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifdef OPENMP
#  include <omp.h>
#endif
#include <mpi.h>

#define ARR_MAX 1024

static unsigned int my_r = 1676;

double do_work(int units) {
  int i, j;
  int r1, r2;
  static double arrA[ARR_MAX], arrB[ARR_MAX];

  for (i=0; i<units; ++i) {
    for (j=0; j<2000; ++j) {
      r1 = (my_r = (my_r * 9631) % 21599);
      r2 = (my_r = (my_r * 9631) % 21599);
      arrA[r1 % ARR_MAX] = arrB[r2 % ARR_MAX];
    }
  }
  r1 = (my_r = (my_r * 9631) % 21599);
  return arrA[r1 % ARR_MAX];
}

double step(int id, int tid, int i) {
  double r;

  /* printf("%d:%d: step %d\n", id, tid, i); */
  r = do_work(100);

  return r;
}

double sequential(int id) {
  int i, tid=0;
  double sum;

  sum = do_work(100);
  #pragma omp parallel private(tid)
  {
    #ifdef OPENMP
    tid = omp_get_thread_num();
    #endif
    #pragma omp for reduction(+:sum)
    for (i=0; i<12; i++) {
      sum += step(id, tid, i);
    }
  }
  sum += do_work(100);

  return sum;
}

void p2p(MPI_Comm comm) {
  int id=0, numprocs, value=42, i, dest;
  MPI_Request req;
  MPI_Status stat;

  MPI_Comm_size(comm, &numprocs);
  MPI_Comm_rank(comm, &id);

  MPI_Barrier(comm);
  for (i=0; i<5; i++) {
    if ( id % 2 == 0 ) {
      value = 42;
      dest = (id+1+numprocs) % numprocs;
      if ( dest != id && dest < numprocs ) {
        MPI_Isend(&value, 1, MPI_INT, dest, 42, comm, &req);
        MPI_Wait(&req, &stat);
      }
    } else {
      value = 0;
      MPI_Irecv(&value, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);
      MPI_Wait(&req, &stat);
    }
  }
  MPI_Barrier(comm);
}

void parallel(MPI_Comm comm) {
  int id=0, numprocs, value=42;
  double sum=0.0, gsum=0.0;

  MPI_Comm_size(comm, &numprocs);
  MPI_Comm_rank(comm, &id);
  MPI_Bcast(&value, 1, MPI_INT, numprocs-1, comm);

  sum += sequential(id);
  MPI_Barrier(comm);
  sum += sequential(id);
  MPI_Barrier(comm);
  sum += sequential(id);
  MPI_Allreduce (&sum, &gsum, 1, MPI_DOUBLE, MPI_SUM, comm);

  p2p(comm);
}

int main(int argc, char* argv[]) {
  int id=0, numprocs;
  int color;
  MPI_Comm local;
  double t1, t2;

  int provided = 0;
#ifdef _OPENMP
  const int required = MPI_THREAD_FUNNELED;
  int ierr = MPI_Init_thread(&argc, &argv, required, &provided);
  if (ierr != MPI_SUCCESS) {
      printf("Abort: MPI_Init_thread unsuccessful: %s\n", strerror(errno));
      MPI_Abort(MPI_COMM_WORLD, ENOSYS);
  }
#else
  int ierr = MPI_Init(&argc, &argv);
  if (ierr != MPI_SUCCESS) {
      printf("Abort: MPI_Init unsuccessful: %s\n", strerror(errno));
      MPI_Abort(MPI_COMM_WORLD, ENOSYS);
  }
#endif

  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  if (id == 0) {
      int version, subversion;
      MPI_Get_version(&version, &subversion);
      printf("%d MPI-%d.%d#%d process(es)", numprocs, version, subversion, provided);
#ifdef _OPENMP
      printf(" with %d OpenMP-%u thread(s)/process on %d processors",
            omp_get_max_threads(), _OPENMP, omp_get_num_procs());
      if (provided < required)
          printf("\nWarning: MPI_Init_thread only provided level %d<%d!", provided, required);
#endif
      printf("\n");
  } else sleep(1);

  printf("%03d: ctest start\n", id);
  color = (id >= numprocs/2);
  MPI_Comm_split(MPI_COMM_WORLD, color, id, &local);

  t1 = MPI_Wtime();
  parallel(MPI_COMM_WORLD);
  parallel(local);
  parallel(MPI_COMM_WORLD);
  t2 = MPI_Wtime();

  MPI_Comm_free(&local);
  MPI_Finalize();
  printf("%03d: ctest end (%12.9f)\n", id, (t2-t1));

  return 0;
}
