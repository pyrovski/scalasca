!****************************************************************************
!*  SCALASCA    http://www.scalasca.org/                                   **
!****************************************************************************
!*  Copyright (c) 1998-2013                                                **
!*  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
!*                                                                         **
!*  See the file COPYRIGHT in the package base directory for details       **
!***************************************************************************/

program mpigenheaders
  include 'mpif.h'
  open(42,file='elg_fmpi_defs.h')
  write (42,*) "#define ELG_MPI_STATUS_SIZE", MPI_STATUS_SIZE
  close(42)

  call gen_elg_types()
  call foo_foo()
end
