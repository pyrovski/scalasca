!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

#ifndef VT_INST
#  define include_VT_inc
#  define call_VTINIT(e)
#  define call_VTSYMDEF(i,n,g,e)
#  define call_VTBEGIN(i,e)
#  define call_VTEND(i,e)
#  define call_VTFINI(e)
#else
#  define include_VT_inc          include "VT.inc"
#  define call_VTINIT(e)          call VTINIT(e)
#  define call_VTSYMDEF(i,n,g,e)  call VTSYMDEF(i,n,g,e)
#  define call_VTBEGIN(i,e)       call VTBEGIN(i,e)
#  define call_VTEND(i,e)         call VTEND(i,e)
#  define call_VTFINI(e)          call VTFINI(e)
#endif

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
  integer :: id, tid, i, ierr
  real :: w
  include_VT_inc

  call_VTBEGIN(1, ierr)
  !write (*,*) id, ":", tid, ": step", i
  w = do_work(100)
  call_VTEND(1, ierr)
end subroutine

subroutine sequential(id)
  integer :: i, id, tid=0, ierr
  integer :: omp_get_thread_num
  include_VT_inc

  call_VTBEGIN(2, ierr)

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

  call_VTEND(2, ierr)
end subroutine

program main
  include 'mpif.h'
  include_VT_inc
  integer :: id=0, numprocs, value=42, ierr

  call_VTINIT(ierr)
  call_VTSYMDEF(1, 'step', 'USR', ierr)
  call_VTSYMDEF(2, 'sequential', 'USR', ierr)
  call_VTSYMDEF(3, 'main', 'USR', ierr)
  call_VTBEGIN(3, ierr)
  
  call MPI_Init(ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, numprocs, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, id, ierr)
  write (*,*) id, ": ftest-vt start"
  call MPI_Bcast(value, 1, MPI_INTEGER, numprocs-1, MPI_COMM_WORLD, ierr)

  call sequential(id)

  call MPI_Barrier(MPI_COMM_WORLD, ierr)
  call MPI_Finalize(ierr)
  write (*,*) id, ": ftest-vt end"

  call_VTEND(3, ierr)
  call_VTFINI(ierr)
end program main
