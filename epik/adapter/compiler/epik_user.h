/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _EPIK_USER_H
#define _EPIK_USER_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - User Instrumentation (C/C++)
 * 
 *-----------------------------------------------------------------------------
 */

  EXTERN void EPIK_User_start(const char* name, const char *file, const int lno);
  EXTERN void EPIK_User_end(const char *name, const char *file, const int lno);
  EXTERN void EPIK_Pause_start(const char *file, const int lno);
  EXTERN void EPIK_Pause_end(const char *file, const int lno);
  EXTERN void EPIK_Flush_trace(const char *file, const int lno);

  #ifdef __cplusplus
    class EPIK_Tracer {
    public:
      EPIK_Tracer(const char* name, const char *file, const int lno)
        : n(name), f(file), l(lno)
      {
          EPIK_User_start(name, file, lno);
      }
      ~EPIK_Tracer() { EPIK_User_end(n, f, l); }
    private:
      const char *n;
      const char *f;
      const int   l;
    };
  #endif

#ifdef EPIK
  /* deprecated compatibility interface */
  EXTERN void ELG_User_start(const char* name, const char *file, const int lno);
  EXTERN void ELG_User_end(const char *name);

  #define EPIK_USER_REG(n,str)  static const char* n=str
  #define EPIK_USER_START(n)    EPIK_User_start(n, __FILE__, __LINE__)
  #define EPIK_USER_END(n)      EPIK_User_end(n, __FILE__, __LINE__)
  #define EPIK_FUNC_START()     EPIK_User_start(__func__, __FILE__, __LINE__)
  #define EPIK_FUNC_END()       EPIK_User_end(__func__, __FILE__, __LINE__)

  #ifdef __cplusplus
    #define EPIK_TRACER(n) EPIK_Tracer EPIK_Trc__(n, __FILE__, __LINE__)
  #endif

  #define EPIK_PAUSE_START()    EPIK_Pause_start(__FILE__, __LINE__)
  #define EPIK_PAUSE_END()      EPIK_Pause_end(__FILE__, __LINE__)
  #define EPIK_FLUSH_TRACE()    EPIK_Flush_trace(__FILE__, __LINE__)
#else
  #define EPIK_TRACER(n)
  #define EPIK_USER_REG(n,str)
  #define EPIK_USER_START(n)
  #define EPIK_USER_END(n)
  #define EPIK_FUNC_START()
  #define EPIK_FUNC_END()

  #define EPIK_PAUSE_START()
  #define EPIK_PAUSE_END()
  #define EPIK_FLUSH_TRACE()
#endif

#endif
