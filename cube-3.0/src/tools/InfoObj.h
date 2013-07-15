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
* \file InfoObj.h
* \brief Provides a general class contains the information about a callpaths. 
Just a wrapper over vector<CallpathType>
*/


#ifndef CUBE_INFOOBJ
#define CUBE_INFOOBJ

#include <cstddef>
#include <vector>

#include "enums.h"

namespace cube
{
  class InfoObj
    {
    protected:
      std::vector<CallpathType> m_types;
      
    public:
/**
* Wrapper for an operator []
*/
      CallpathType operator[](std::size_t i) const
      {
	return m_types[i];
      }

/**
* Constructor. Creates "n" USR calls.
*/
      InfoObj(std::size_t n)
	: m_types(n, USR)
	{}
    };
}

#endif
