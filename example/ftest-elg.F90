!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

#include "elg_user.inc"

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

  ELG_USER_START('step')
  !write (*,*) id, ":", tid, ": step", i
  w = do_work(100)
  ELG_USER_END('step')
end subroutine

subroutine sequential(id)
  integer :: i, id, tid=0
  integer :: omp_get_thread_num

  ELG_USER_START('sequential')

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

  ELG_USER_END('sequential')
end subroutine

program main
  include 'elg_ftopol.inc'
  include 'mpif.h'
  integer :: id=0, numprocs, value=42, ierr

  ELG_USER_START('main')
  
  call MPI_Init(ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, numprocs, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, id, ierr)
  write (*,*) id, ": ftest-elg start"
  call MPI_Bcast(value, 1, MPI_INTEGER, numprocs-1, MPI_COMM_WORLD, ierr)

  call ELGF_Cart_create(2, (numprocs+1)/2, 0, 1, 1, 0)
  call ELGF_Cart_coords(MOD(id,2), id/2, 0)

  call sequential(id)

  call MPI_Barrier(MPI_COMM_WORLD, ierr)
  call MPI_Finalize(ierr)
  write (*,*) id, ": ftest-elg end"

  ELG_USER_END('main')
end program main
