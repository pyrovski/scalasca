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

#ifndef EARL_METRIC_H
#define EARL_METRIC_H

/*
 *----------------------------------------------------------------------------
 *
 * class Metric
 *
 * Performance metric (e.g., hardware counter)
 *
 *----------------------------------------------------------------------------
 */

#include <string>

namespace earl
{
  class Metric
  {
  public:

    Metric (long        id,
	    std::string name,
	    std::string descr, 
	    std::string type,     
	    std::string mode,     
	    std::string ival) :
      id(id), name(name), descr(descr), type(type), mode(mode), ival(ival) {}

    long        get_id()    const { return id; }
    std::string get_name()  const { return name; }
    std::string get_descr() const { return descr; }
    std::string get_type()  const { return type; }    
    std::string get_mode()  const { return mode; }    
    std::string get_ival()  const { return ival; }
 
  private:
  
    long        id;
    std::string name;      // one of (*)
    std::string descr; 
    std::string type;      // either "INTEGER" or "FLOAT"
    std::string mode;      // either "COUNTER", "RATE",  "SAMPLE"
    std::string ival;      // either "START", "LAST", "NEXT", or "NONE"
  };
}

/*
  (*) performance metrics

  "L1_ACCESS"
  "L1_I_ACCESS"
  "L1_D_ACCESS"
 
  "L1_READ"
  "L1_I_READ"
  "L1_D_READ"
  
  "L1_WRITE"
  "L1_I_WRITE"
  "L1_D_WRITE"
  
  "L1_HIT"
  "L1_I_HIT"
  "L1_D_HIT"
  
  "L1_MISS"
  "L1_I_MISS"
  "L1_D_MISS"
  
  "L2_ACCESS"
  "L2_I_ACCESS"
  "L2_D_ACCESS"
  
  "L2_READ"
  "L2_I_READ"
  "L2_D_READ"
  
  "L2_WRITE"
  "L2_I_WRITE"
  "L2_D_WRITE"
  
  "L2_HIT"
  "L2_I_HIT"
  "L2_D_HIT"
  
  "L2_MISS"
  "L2_I_MISS"
  "L2_D_MISS"

  "L3_ACCESS"
  "L3_I_ACCESS"
  "L3_D_ACCESS"
  
  "L3_READ"
  "L3_I_READ"
  "L3_D_READ"
  
  "L3_WRITE"
  "L3_I_WRITE"
  "L3_D_WRITE"
  
  "L3_HIT"
  "L3_I_HIT"
  "L3_D_HIT"
  
  "L3_MISS"
  "L3_I_MISS"
  "L3_D_MISS"
  
  "TLB_MISS"
  "TLB_I_MISS"
  "TLB_D_MISS"
  
  "INSTRUCTION"
  "INTEGER"
  "FLOATING_POINT"
  "LOAD_STORE"
  "LOAD"
  "STORE"
  "COND_STORE"
  "COND_STORE_SUCCESS"
  "COND_STORE_UNSUCCESS"
  "COND_BRANCH"
  "COND_BRANCH_TAKEN"
  "COND_BRANCH_NOTTAKEN"
  "COND_BRANCH_PRED"
  "COND_BRANCH_MISPRED"
  
  "CYCLES"
  
  "INTEGER_UNIT_IDLE"
  "FLOAT_UNIT_IDLE"
  "BRANCH_UNIT_IDLE"
  "LOADSTORE_UNIT_IDLE"

  "STALL_MEMORY_ACCESS"
  "STALL_MEMORY_READ"
  "STALL_MEMORY_WRITE"
*/

#endif

