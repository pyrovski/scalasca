/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_READCB_H
#define PEARL_READCB_H


#include <string>
#include <vector>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    readcb.h
 * @ingroup PEARL_base
 * @brief   Definition of helper classes which are passed to the EPILOG
 *          read callbacks.
 *
 * This header file provides the definition of the two helper classes
 * DefsCbData and TraceCbData which are passed as user data to the
 * EPILOG file read callbacks.
 */
/*-------------------------------------------------------------------------*/


namespace pearl {

//--- Forward declarations --------------------------------------------------

class GlobalDefs;
class LocalTrace;
class MappingTable;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @struct  DefsCbData
 * @ingroup PEARL_base
 * @brief   Callback data object used while reading definition records.
 */
/*-------------------------------------------------------------------------*/

struct DefsCbData
{
  /// @name Constructors & destructor
  /// @{

  DefsCbData(GlobalDefs& defs);

  /// @}
  /// @name Mapping functions
  /// @{

  std::string get_string(ident_t id) const;
  std::string get_file  (ident_t id) const;

  /// @}


  /// %GlobalDefs object storing the definitions read
  GlobalDefs& m_defs;

  /// Number of "open" string continuation records
  uint8_t m_strcnt;

  /// Current string ID, used for string continuation records
  ident_t m_strid;

  /// Size of MPI_COMM_WORLD (always 1 in case of serial/OpenMP experiments)
  uint32_t m_worldsize;

  /// Mapping table string ID |-@> string
  std::vector<std::string> m_strmap;

  /// Mapping table file ID |-@> file name
  std::vector<std::string> m_filemap;

  /// Stores the what() string of exceptions thrown in callbacks,
  /// empty if everything was ok
  std::string m_message;
};


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @struct  TraceCbData
 * @ingroup PEARL_base
 * @brief   Callback data object used while reading event trace records.
 */
/*-------------------------------------------------------------------------*/

struct TraceCbData
{
  /// @name Constructors & destructor
  /// @{

  TraceCbData(const GlobalDefs&   defs,
                    LocalTrace&   trace,
                    MappingTable& table,
                    uint32_t      version,
                    uint32_t      tid,
                    int32_t       sionid);

  /// @}


  /// %GlobalDefs object providing the global definitions
  const GlobalDefs& m_defs;

  /// %LocalTrace object storing the event representations read
  LocalTrace& m_trace;

  /// %MappingTable object used for converting local into global identifiers
  MappingTable& m_table;

  /// EPILOG file format version
  uint32_t m_version;

  /// %Location identifier for the processed trace file
  ident_t m_location;

  /// %Thread identifier in case of OpenMP, always 0 for MPI
  uint32_t m_tid;

  /// %Id of SION file, if opened with sion >=0
  int32_t m_sionid;

  /// %Last request identifier
  ident_t m_reqid;

  /// Stores the what() string of exceptions thrown in callbacks,
  /// empty if everything was ok
  std::string m_message;
};


}   /* namespace pearl */


#endif   /* !PEARL_READCB_H */
