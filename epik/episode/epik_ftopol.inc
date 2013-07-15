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
!*  EPIK Library
!*
!*  - Interface for defining topologies
!* 
!*-----------------------------------------------------------------------------

#ifndef EPIKF_CART_CREATE

#ifdef EPIK
#  define EPIKF_CART_CREATE(topology_index, name, num_dims)              topology_index = EPIKF_CART_CREATE(name, num_dims)
#  define EPIKF_CART_COMMIT(topology_index)                              call EPIKF_CART_COMMIT(topology_index)
#  define EPIKF_CART_FREE(topology_index)                                call EPIKF_CART_FREE(topology_index)
#  define EPIKF_CART_ADD_DIM(topology_index, size, periodic, name)       call EPIKF_CART_ADD_DIM(topology_index, size, periodic, name)
#  define EPIKF_CART_SET_COORDS(topology_index, coords)                  call EPIKF_CART_SET_COORDS(topology_index, coords)
#  define EPIKF_CART_COORDS_COMMIT(topology_index)                       call EPIKF_CART_COORDS_COMMIT(topology_index)
#else
#  define EPIKF_CART_CREATE(topology_index, name, num_dims)
#  define EPIKF_CART_COMMIT(topology_index)
#  define EPIKF_CART_FREE(topology_index)
#  define EPIKF_CART_ADD_DIM(topology_index, size, periodic, name)
#  define EPIKF_CART_SET_COORDS(topology_index, coords)
#  define EPIKF_CART_COORDS_COMMIT(topology_index)
#endif

#endif

interface  
  integer function EPIKF_Cart_create (name, num_dims)
    character(*), intent(in) :: name
    integer(kind=4), intent(in) :: num_dims
  end function EPIKF_Cart_create
  
  subroutine EPIKF_Cart_commit(topology_index)
    integer(kind=4), intent(in) :: topology_index    
  end subroutine EPIKF_Cart_commit
  
  subroutine EPIKF_Cart_free(topology_index)
    integer(kind=4), intent(in) :: topology_index
  end subroutine EPIKF_Cart_free

  subroutine EPIKF_Cart_add_dim(topology_index, size, periodic, name)
    integer(kind=4), intent(in) :: topology_index, size
    integer(kind=4), intent(in) :: periodic
    character(*),    intent(in) :: name
  end subroutine EPIKF_Cart_add_dim

  subroutine EPIKF_Cart_set_coords(topology_index, coords)
    integer(kind=4), intent(in) :: topology_index
    integer(kind=4), intent(in) :: coords(*)
  end subroutine EPIKF_Cart_set_coords

  subroutine EPIKF_Cart_coords_commit(topology_index)
    integer(kind=4), intent(in) :: topology_index
  end subroutine EPIKF_Cart_coords_commit
  
end interface
