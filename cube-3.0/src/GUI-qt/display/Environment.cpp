/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cctype>
#include <cstring>
#include <cstdlib>

#include "Environment.h"

using namespace std;


/**
 * Converts a string read from an environment variable to a boolean value.
 * Any positive integer as well as the character strings "yes" and "true"
 * (case-insensitive) are are recognized as @c true, any other value as
 * @c false.
 */
bool env_str2bool(const char* str)
{
    char strbuf[128];
    char* ptr = strbuf;

    if (!str) return false;

    strncpy(strbuf, str, 128);
    while ( *ptr )
    {
        *ptr = tolower(*ptr);
        ++ptr;
    }

    if ( strcmp(strbuf, "yes") == 0  ||
         strcmp(strbuf, "true") == 0 ||
         atoi(strbuf) > 0 )
        return true;

    return false;
}
