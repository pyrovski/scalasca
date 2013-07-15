/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_REGION_H
#define EARL_REGION_H

#define EARL_REGION_GENERATOR   "TRACER"
#define EARL_REGION_TRACING     "TRACING"
#define EARL_REGION_NO_LOGGING  "NO_LOGGING"

#include "NamedObj.h"

/*
 *----------------------------------------------------------------------------
 *
 * class Region
 *
 * Source-code region.
 *
 *----------------------------------------------------------------------------
 */


namespace earl
{
  class Region : public NamedObj
  {
  public:

    Region(long        id,
	   std::string name,
	   std::string file,
	   long        begln,
	   long        endln,
	   std::string descr,
	   std::string rtype) : 
      id(id), name(name), file(file), begln(begln), endln(endln), descr(descr), rtype(rtype) {}

    long        get_id()    const { return id; }
    std::string get_name()  const { return name; }
    std::string get_file()  const { return file; }
    long        get_begln() const { return begln; }
    long        get_endln() const { return endln; }
    std::string get_descr() const { return descr; }
    std::string get_rtype() const { return rtype; }
 
    bool operator== (const Region& reg ) { return reg.id == id; }
    bool operator!= (const Region& reg ) { return reg.id != id; }
  private:
  
    long        id;
    std::string name;
    std::string file;
    long        begln;
    long        endln;
    std::string descr;
    std::string rtype; // any one of (*)
  };
}
#endif


/*

  (*) region types

  "FUNCTION"
  "LOOP"
  "USER_REGION"
  "OMP_PARALLEL"
  "OMP_LOOP"
  "OMP_SECTIONS"
  "OMP_SECTION"
  "OMP_WORKSHARE"
  "OMP_SINGLE"
  "OMP_MASTER"
  "OMP_CRITICAL"
  "OMP_ATOMIC"
  "OMP_BARRIER"
  "OMP_IBARRIER" (implicit barrier)
  "OMP_FLUSH"           
  "OMP_CRITICAL_SBLOCK"
  "OMP_SINGLE_SBLOCK"     
  "UNKNOWN" 
*/
