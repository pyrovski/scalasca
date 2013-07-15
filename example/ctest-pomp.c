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

  #pragma pomp inst begin(pomp$step)

  /* printf("%d:%d: step %d\n", id, tid, i); */
  r = do_work(100);

  #pragma pomp inst end(pomp$step)
  return r;
}

double sequential(int id) {
  int i, tid=0;
  double sum;

  #pragma pomp inst begin(pomp$sequential)

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

  #pragma pomp inst end(pomp$sequential)
  return sum;
}

void p2p(MPI_Comm comm) {
  int id=0, numprocs, value=42, i, dest;
  MPI_Request req;
  MPI_Status stat;

  #pragma pomp inst begin(pomp$p2p)

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

  #pragma pomp inst end(pomp$p2p)
}

void parallel(MPI_Comm comm) {
  int id=0, numprocs, value=42;
  double sum=0.0, gsum=0.0;

  #pragma pomp inst begin(pomp$parallel)

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

  #pragma pomp inst end(pomp$parallel)
}

int main(int argc, char* argv[]) {
  int id=0, numprocs;
  int color;
  MPI_Comm local;
  double t1, t2;

  #pragma pomp inst init
  #pragma pomp inst begin(pomp$main)

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  printf("%03d: ctest-pomp start\n", id);
  color = (id >= numprocs/2);
  MPI_Comm_split(MPI_COMM_WORLD, color, id, &local);

  t1 = MPI_Wtime();
  parallel(MPI_COMM_WORLD);
  parallel(local);
  parallel(MPI_COMM_WORLD);
  t2 = MPI_Wtime();

  MPI_Comm_free(&local);
  MPI_Finalize();
  printf("%03d: ctest-pomp end (%12.9f)\n", id, (t2-t1));

  #pragma pomp inst end(pomp$main)
  return 0;
}
