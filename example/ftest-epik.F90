!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

#include "epik_user.inc"

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
  EPIK_FUNC_REG("step")
  EPIK_FUNC_START()

  !write (*,*) id, ":", tid, ": step", i
  w = do_work(100)

  EPIK_FUNC_END()
end subroutine

subroutine sequential(id)
  integer :: i, id, tid=0
  integer :: omp_get_thread_num
  EPIK_FUNC_REG("sequential")
  EPIK_USER_REG(r_loop,"<<loop>>")
  EPIK_FUNC_START()

  !$OMP PARALLEL PRIVATE(tid) SHARED(id)
#ifdef OPENMP
    tid = omp_get_thread_num()
#endif
    !$OMP DO
    do i=1,12
      EPIK_USER_START(r_loop)
      call step(id, tid, i)
      EPIK_USER_END(r_loop)
    enddo
    !$OMP END DO
  !$OMP END PARALLEL

  EPIK_FUNC_END()
end subroutine

program main
#include "epik_ftopol.inc"
  include 'mpif.h'
  integer :: id=0, numprocs, value=42, ierr
  integer :: topo_id, coords(4)

  EPIK_USER_REG(r_init, "<<init>>")
  EPIK_USER_REG(r_fini, "<<fini>>")
  EPIK_FUNC_REG("main")
  EPIK_FUNC_START()
  
  EPIK_USER_START(r_init)

  call MPI_Init(ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, numprocs, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, id, ierr)
  write (*,*) id, ": ftest-epik start"

  EPIKF_CART_CREATE(topo_id, "F Test Topology", 2)
  EPIKF_CART_ADD_DIM(topo_id, 2, 1, "x")
  EPIKF_CART_ADD_DIM(topo_id, (numprocs+1)/2, 1, "y")
  EPIKF_CART_COMMIT(topo_id)

  coords(1)=MOD(id,2)
  coords(2)=id/2
  EPIKF_CART_SET_COORDS(topo_id, coords)
  EPIKF_CART_COORDS_COMMIT(topo_id)
  EPIKF_CART_FREE(topo_id)

  call MPI_Bcast(value, 1, MPI_INTEGER, numprocs-1, MPI_COMM_WORLD, ierr)
  EPIK_USER_END(r_init)

  call sequential(id)

  EPIK_USER_START(r_fini)
  call MPI_Barrier(MPI_COMM_WORLD, ierr)
  call MPI_Finalize(ierr)
  write (*,*) id, ": ftest-epik end"
  EPIK_USER_END(r_fini)

  EPIK_FUNC_END()
end program main
