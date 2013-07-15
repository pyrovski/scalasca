/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SILAS_UTIL_H
#define SILAS_UTIL_H

#include <string>
#include <algorithm>

extern void log(const char* fmt, ...);

namespace silas {
    /**
     * @brief helper function to convert a string to all lowercase * letters
     * @param str input string
     * @return converted string
     */
    inline std::string lowercase(const std::string& str) {
        std::string result(str);
        std::transform(str.begin(), str.end(), result.begin(), (int(*)(int)) tolower);

        return result;
    }
}

#endif
