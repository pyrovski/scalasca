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
!*  - User Instrumentation
!* 
!*-----------------------------------------------------------------------------

#ifdef EPILOG
#  define EPIK EPILOG
#  include "epik_user.inc"
#  define ELG_USER_START(n) EPIK_USER_START(n)
#  define ELG_USER_END(n)   EPIK_USER_END(n)
#else
#  define ELG_USER_START(n)
#  define ELG_USER_END(n)
#endif
