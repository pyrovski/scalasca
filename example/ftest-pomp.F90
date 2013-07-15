!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

function do_work(units)
  integer :: i, j, units
  real :: x1, x2, do_work

  do i=1,units
    do j=1,10000
      x1 = j * 3.1415;
      x2 = x1 * 42.0 * (x1 - 42.0);
    enddo
  enddo
  do_work = x2
end function

subroutine step(id, tid, i)
  integer :: id, tid, i
  real :: w

  !POMP$ INST BEGIN(pomp$step)
  !write (*,*) id, ":", tid, ": step", i
  w = do_work(100)
  !POMP$ INST END(pomp$step)
end subroutine

subroutine sequential(id)
  integer :: i, id, tid=0
  integer :: omp_get_thread_num

  !POMP$ INST BEGIN(pomp$sequential)

  !$OMP PARALLEL PRIVATE(tid) SHARED(id)
#ifdef OPENMP
    tid = omp_get_thread_num()
#endif
    !$OMP DO
    do i=1,12
      call step(id, tid, i)
    enddo
    !$OMP END DO
  !$OMP END PARALLEL

  !POMP$ INST END(pomp$sequential)
end subroutine

program main
  include 'mpif.h'
  integer :: id=0, numprocs, value=42, ierr

  !POMP$ INST INIT
  !POMP$ INST BEGIN(pomp$main)
  
  call MPI_Init(ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, numprocs, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, id, ierr)
  write (*,*) id, ": ftest-pomp start"
  call MPI_Bcast(value, 1, MPI_INTEGER, numprocs-1, MPI_COMM_WORLD, ierr)

  call sequential(id)

  call MPI_Barrier(MPI_COMM_WORLD, ierr)
  call MPI_Finalize(ierr)
  write (*,*) id, ": ftest-pomp end"

  !POMP$ INST END(pomp$main)
end program main
