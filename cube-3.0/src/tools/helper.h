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
* \file helper.h
* \brief Declares a set of functions to handle with CallPath types 
    (Transform string->callpathtype), Factors
*/
#ifndef CUBE_HELPER
#define CUBE_HELPER

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <map>

#include "Region.h"
#include "Cube.h"
#include "Cnode.h"
#include "enums.h"

namespace cube
{   
    std::string Callpathtype2String(CallpathType);

    CallpathType String2Callpathtype(std::string);

    std::vector<CallpathType> classify_callpaths(Cube*);

    std::string get_callpath_for_cnode(const Cnode&);

    unsigned long long TypeFactor(std::string);

    double get_atotalt(Cube*, std::string);
/**
* Finds a maximum value in a vector and returns a pair (value, index) 
*
* Note: Similar function is in cube_nodeview.cpp|.h
*/
    template<class T>
    std::pair<T, size_t> find_max(std::vector<T> v)
    {
      typename std::vector<T>::iterator it;
      it = std::max_element(v.begin(), v.end());
      return std::pair<T, size_t>(*it, it - v.begin());
    }
}

#endif
