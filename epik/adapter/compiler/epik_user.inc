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
!*  EPIK Library (Event Processing Interface Kit)
!*
!*  - User Instrumentation (Fortran)
!* 
!*-----------------------------------------------------------------------------

#ifdef EPIK
#  define __EPK_LINE()          __LINE__
#  define __EPK_LINE__          __EPK_LINE()_4
#  define __LINE___4            __LINE__

#  define EPIK_USER_REG(n,str)  character*(32)::n=str//char(0)
#  define EPIK_USER_START(n)    call EPIK_User_start(n, __FILE__, __EPK_LINE__)
#  define EPIK_USER_END(n)      call EPIK_User_end(n, __FILE__, __EPK_LINE__)

#  define EPIK_FUNC_REG(str)    EPIK_USER_REG(epik_func_id__,str)
#  define EPIK_FUNC_START()     EPIK_USER_START(epik_func_id__)
#  define EPIK_FUNC_END()       EPIK_USER_END(epik_func_id__)

#  define EPIK_PAUSE_START()    call EPIK_Pause_start(__FILE__, __EPK_LINE__)
#  define EPIK_PAUSE_END()      call EPIK_Pause_end(__FILE__, __EPK_LINE__)
#  define EPIK_FLUSH_TRACE()    call EPIK_Flush_trace(__FILE__, __EPK_LINE__)
#else
#  define EPIK_USER_REG(n,str)
#  define EPIK_USER_START(n)
#  define EPIK_USER_END(n)

#  define EPIK_FUNC_REG(str)
#  define EPIK_FUNC_START()
#  define EPIK_FUNC_END()

#  define EPIK_PAUSE_START()
#  define EPIK_PAUSE_END()
#  define EPIK_FLUSH_TRACE()
#endif

