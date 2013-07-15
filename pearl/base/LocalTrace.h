/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2006-2013                                                **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_LOCALTRACE_H
#define PEARL_LOCALTRACE_H


#if defined PEARL_USE_LIST
#  include "LIST/LocalTrace.h"
#elif defined PEARL_USE_CCCG
#  include "CCCG/LocalTrace.h"
#else
#  error "No trace data storage backend selected!"
#endif


#endif   /* !PEARL_LOCALTRACE_H */
