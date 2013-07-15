/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cctype>
#include <functional>

#include <elg_defs.h>

#include "Error.h"
#include "Region.h"

using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

namespace
{

struct fo_tolower : public std::unary_function<int,int> {
  int operator()(int x) const {
    return std::tolower(x);
  }
};

string lowercase(const string& str)
{
  string result(str);

  transform(str.begin(), str.end(), result.begin(), fo_tolower());

  return result;
}


uint32_t classify_mpi(const string& name)
{
  // MPI setup
  if (name == "init" || name == "init_thread")
    return Region::CLASS_MPI | Region::CAT_MPI_SETUP | Region::TYPE_MPI_INIT;
  if (name == "finalize")
    return Region::CLASS_MPI | Region::CAT_MPI_SETUP | Region::TYPE_MPI_FINALIZE;

  // MPI collective
  if (name == "barrier")
    return Region::CLASS_MPI | Region::CAT_MPI_COLLECTIVE | Region::TYPE_MPI_BARRIER;
  if (name == "bcast" || name == "scatter" || name == "scatterv")
    return Region::CLASS_MPI | Region::CAT_MPI_COLLECTIVE | Region::TYPE_MPI_ONE_TO_N;
  if (name == "reduce" || name == "gather" || name == "gatherv")
    return Region::CLASS_MPI | Region::CAT_MPI_COLLECTIVE | Region::TYPE_MPI_N_TO_ONE;
  if (name == "allgather" || name == "allgatherv" || name == "allreduce" ||
      name == "alltoall" || name == "alltoallv" || name == "alltoallw" ||
      name == "reduce_scatter" || name == "reduce_scatter_block")
    return Region::CLASS_MPI | Region::CAT_MPI_COLLECTIVE | Region::TYPE_MPI_N_TO_N;
  if (name == "scan" || name == "exscan")
    return Region::CLASS_MPI | Region::CAT_MPI_COLLECTIVE | Region::TYPE_MPI_PARTIAL;

  // MPI point-to-point
  if (name.compare(0, 8, "sendrecv") == 0)
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_SENDRECV | Region::MODE_MPI_NONBLOCKING;

  if (name.find("send") != string::npos) {
    uint32_t result = Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_SEND;

    if (name[0] == 'i')
      result |= Region::MODE_MPI_NONBLOCKING;
    else if (name.find("init") != string::npos)
      result |= Region::MODE_MPI_PERSISTENT;

    if (name.find("ssend") != string::npos)
      result |= Region::MODE_MPI_SYNCHRONOUS;
    else if (name.find("bsend") != string::npos)
      result |= Region::MODE_MPI_BUFFERED;
    else if (name.find("rsend") != string::npos)
      result |= Region::MODE_MPI_READY;
    else
      result |= Region::MODE_MPI_STANDARD;

    return result;
  }

  if (name == "recv")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_RECV;
  if (name == "irecv")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_RECV | Region::MODE_MPI_NONBLOCKING;
  if (name == "recv_init")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_RECV | Region::MODE_MPI_PERSISTENT;

  if (name == "test" || name == "testany")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_TEST | Region::MODE_MPI_SINGLE;
  if (name == "testall" || name == "testsome")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_TEST | Region::MODE_MPI_MANY;

  if (name == "wait" || name == "waitany")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_WAIT | Region::MODE_MPI_SINGLE;
  if (name == "waitall" || name == "waitsome")
    return Region::CLASS_MPI | Region::CAT_MPI_P2P | Region::TYPE_MPI_WAIT | Region::MODE_MPI_MANY;

  if (name.compare(0, 6, "buffer") == 0 ||
      name == "cancel" ||
      name == "get_count" ||
      name.find("probe") != string::npos ||
      name.compare(0, 7, "request") == 0 ||
      name.compare(0, 5, "start") == 0)
    return Region::CLASS_MPI | Region::CAT_MPI_P2P;

  // MPI I/O
  if (name.compare(0, 4, "file") == 0)
    return Region::CLASS_MPI | Region::CAT_MPI_IO;

  // MPI RMA
  if (name == "accumulate" || name == "put")
    return Region::CLASS_MPI | Region::CAT_MPI_RMA | Region::TYPE_MPI_RMA_COMM | Region::MODE_RMA_PUT;
  if (name == "get")
    return Region::CLASS_MPI | Region::CAT_MPI_RMA | Region::TYPE_MPI_RMA_COMM | Region::MODE_RMA_GET;
  if (name.compare(0, 3, "win") == 0)
  {
    uint32_t result = Region::CLASS_MPI | Region::CAT_MPI_RMA;
    string   mode   = name.substr(4);

    if (mode == "fence")
      return result | Region::TYPE_MPI_RMA_COLL | Region::MODE_RMA_FENCE;
    if (mode == "create")
      return result | Region::TYPE_MPI_RMA_COLL | Region::MODE_RMA_WIN_CREATE;
    if (mode == "free")
      return result | Region::TYPE_MPI_RMA_COLL | Region::MODE_RMA_WIN_FREE;

    if (mode == "start")
      return result | Region::TYPE_MPI_RMA_GATS | Region::MODE_RMA_START;
    if (mode == "complete")
      return result | Region::TYPE_MPI_RMA_GATS | Region::MODE_RMA_COMPLETE;
    if (mode == "post")
      return result | Region::TYPE_MPI_RMA_GATS | Region::MODE_RMA_POST;
    if (mode == "wait")
      return result | Region::TYPE_MPI_RMA_GATS | Region::MODE_RMA_WAIT;
    if (mode == "test")
      return result | Region::TYPE_MPI_RMA_GATS | Region::MODE_RMA_TEST;

    if (mode == "lock")
      return result | Region::TYPE_MPI_RMA_PASSIVE | Region::MODE_RMA_LOCK;
    if (mode == "unlock")
      return result | Region::TYPE_MPI_RMA_PASSIVE | Region::MODE_RMA_UNLOCK;
  }

  return Region::CLASS_MPI;
}


uint32_t classify_omp(const string& name)
{
  if (name.compare(0, 14, "!$omp parallel") == 0)
    return Region::CLASS_OMP | Region::CAT_OMP_PARALLEL;

  if (name.compare(0, 13, "!$omp barrier") == 0)
    return Region::CLASS_OMP | Region::CAT_OMP_SYNC
           | Region::TYPE_OMP_BARRIER | Region::MODE_OMP_EXPLICIT;

  if (name.compare(0, 14, "!$omp ibarrier") == 0)
    return Region::CLASS_OMP | Region::CAT_OMP_SYNC
           | Region::TYPE_OMP_BARRIER | Region::MODE_OMP_IMPLICIT;

  return Region::CLASS_OMP;
}


uint32_t classify(const string& name, const string& file)
{
  // Measurement related regions
  if (file == "EPIK")
    return Region::CLASS_INTERNAL;

  // MPI regions
  if (file == "MPI")
    return classify_mpi(lowercase(name.substr(4)));

  // OpenMP regions
  if (name.compare(0, 5, "!$omp") == 0 || file == "OMP")
    return classify_omp(lowercase(name));

  // User regions
  return Region::CLASS_USER;
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class Region
 *
 *---------------------------------------------------------------------------
 */


//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets its data fields to the given values.
 *
 * @param id          %Region identifier
 * @param name        %Region name
 * @param file        Name of file in which the region is defined
 * @param begin       Begin line number
 * @param end         End line number
 * @param description %Region description
 * @param type        %Region type
 */
Region::Region(ident_t            id,
               const std::string& name,
               const std::string& file,
               line_t             begin,
               line_t             end,
               const std::string& description,
               uint8_t            type)
  : NamedObject(id, name),
    m_file(file),
    m_begin(begin),
    m_end(end),
    m_description(description),
    m_type(type)
{
  m_class = classify(name, file);
}


//--- Get region information ------------------------------------------------

/**
 * Returns the name of the file in which the region is defined. If this
 * information is not available, an empty string is returned.
 *
 * @return File name
 */
string Region::get_file() const
{
  return m_file;
}


/**
 * Returns the begin line number of the source-code region. If this
 * information is not available, PEARL_NO_LINE is returned.
 *
 * @return Begin line number
 */
line_t Region::get_begin() const
{
  return m_begin;
}


/**
 * Returns the end line number of the source-code region. If this
 * information is not available, PEARL_NO_LINE is returned.
 *
 * @return End line number
 */
line_t Region::get_end() const
{
  return m_end;
}


/**
 * Returns the region description text associated with the source-code
 * region. If this information is not available, an empty string is
 * returned.
 *
 * @return %Region description.
 */
string Region::get_description() const
{
  return m_description;
}


/**
 * Returns the region type, which can be one of the following strings:
 *   - "FUNCTION"
 *   - "LOOP"
 *   - "USER_REGION" (user-defined region)
 *   - "FUNCTION_COLL_BARRIER"
 *   - "FUNCTION_COLL_ONE2ALL"
 *   - "FUNCTION_COLL_ALL2ONE"
 *   - "FUNCTION_COLL_ALL2ALL"
 *   - "FUNCTION_COLL_OTHER"
 *   - "OMP_PARALLEL"
 *   - "OMP_LOOP" (for/do construct)
 *   - "OMP_SECTIONS" (sections construct)
 *   - "OMP_SECTION" (individual section inside a sections construct)
 *   - "OMP_WORKSHARE"
 *   - "OMP_SINGLE"
 *   - "OMP_MASTER"
 *   - "OMP_CRITICAL"
 *   - "OMP_ATOMIC"
 *   - "OMP_BARRIER"
 *   - "OMP_IBARRIER" (implicit barrier)
 *   - "OMP_FLUSH"
 *   - "OMP_CRITICAL_SBLOCK" (body of critical construct)
 *   - "OMP_SINGLE_SBLOCK" (body of single construct)
 *   - "UNKNOWN"
 *
 * @return %Region type
 */
string Region::get_type() const
{
  string type;

  switch(m_type) {
    case ELG_UNKNOWN:    
      type = "UNKNOWN";
      break;

    case ELG_FUNCTION:              
      type = "FUNCTION";
      break;

    case ELG_FUNCTION_COLL_BARRIER:
      type = "FUNCTION_COLL_BARRIER";
      break;

    case ELG_FUNCTION_COLL_ONE2ALL:
      type = "FUNCTION_COLL_ONE2ALL";
      break;

    case ELG_FUNCTION_COLL_ALL2ONE:
      type = "FUNCTION_COLL_ALL2ONE";
      break;

    case ELG_FUNCTION_COLL_ALL2ALL:
      type = "FUNCTION_COLL_ALL2ALL";
      break;

    case ELG_FUNCTION_COLL_OTHER:
      type = "FUNCTION_COLL_OTHER";
      break;

    case ELG_LOOP:                  
      type = "LOOP";
      break;

    case ELG_USER_REGION:                  
      type = "USER_REGION";
      break;

    case ELG_OMP_PARALLEL:          
      type = "OMP_PARALLEL";
      break;

    case ELG_OMP_LOOP:             
      type = "OMP_LOOP";
      break;

    case ELG_OMP_SECTIONS:          
      type = "OMP_SECTIONS";
      break;

    case ELG_OMP_SECTION:          
      type = "OMP_SECTION";
      break;

    case ELG_OMP_WORKSHARE:          
      type = "OMP_WORKSHARE";
      break;

    case ELG_OMP_SINGLE:           
      type = "OMP_SINGLE";
      break;

    case ELG_OMP_MASTER:           
      type = "OMP_MASTER";
      break;

    case ELG_OMP_CRITICAL:         
      type = "OMP_CRITICAL";
      break;

    case ELG_OMP_ATOMIC:         
      type = "OMP_ATOMIC";
      break;

    case ELG_OMP_BARRIER:                    
      type = "OMP_BARRIER";
      break;

    case ELG_OMP_IBARRIER:                   
      type = "OMP_IBARRIER";
      break;

    case ELG_OMP_FLUSH:                      
      type = "OMP_FLUSH";
      break;

    case ELG_OMP_CRITICAL_SBLOCK:                    
      type = "OMP_CRITICAL_SBLOCK";
      break;

    case ELG_OMP_SINGLE_SBLOCK:                      
      type = "OMP_SINGLE_SBLOCK";
      break;

    default:
      throw FatalError("Unknown region type.");
  }

  return type;
}
