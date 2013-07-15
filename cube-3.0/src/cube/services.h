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
* \brief Provides a set of global service operations for cube library.
*/
#ifndef CUBE_SERVICES_H
#define CUBE_SERVICES_H 0

using namespace std;


namespace services {

std::string escapeToXML(std::string);
std::string escapeFromXML(std::string);

int check_file(const char *);

}

enum ReplaceDirection { SERVICES_REPLACE_FORWARD, SERVICES_REPLACE_BACKWARD};
std::string replaceSymbols(unsigned from, unsigned to, std::string  str, ReplaceDirection direction);

#endif
