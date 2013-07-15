!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  Copyright (c) 2003-2008                                                **
!*  University of Tennessee, Innovative Computing Laboratory               **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

!*-----------------------------------------------------------------------------
!*
!*  EPILOG Library (Event Processing, Investigating, and Logging)
!*
!*  - Interface for defining topologies
!* 
!*-----------------------------------------------------------------------------

! ELGF_CART_CREATE() defines the user cartesian topology
! the first three parameters define number of processes
! in each dimension and the next three parameters define
! the periodicity in each dimension.
! ELGF_CART_COORDS() defines the Cartesian coordinates of
! the current process in the last defined topology.

interface
  subroutine ELGF_CART_CREATE (dim0, dim1, dim2, per0, per1, per2)
    integer(kind=4), intent(in) :: dim0, dim1, dim2
    integer(kind=4), intent(in) :: per0, per1, per2
  end subroutine ELGF_CART_CREATE
  
  subroutine ELGF_CART_COORDS (coord0, coord1, coord2)
    integer(kind=4), intent(in) :: coord0, coord1, coord2
  end subroutine ELGF_CART_COORDS
  
end interface
