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
/**
* \file services.h
* \brief Provides a set of global service operations for tools.
"mean" and so on.
*/
#ifndef CUBE_SERVICES_H
#define CUBE_SERVICES_H 0

char *  services_escape_to_xml(char *);
// char * service_escape_from_xml(char *);
int    service_check_file(const char *);

char * __replace_symbols2(char * str);
int services_check_file(const char * filename);


char*
cubew_strdup( const char* str );

#endif

