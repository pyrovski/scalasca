/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MAPPINGTABLE_H
#define PEARL_MAPPINGTABLE_H


#include <iosfwd>
#include <map>
#include <string>

#include <epk_idmap.h>
#include <elg_types.h>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MappingTable.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class MappingTable.
 *
 * This header file provides the declaration of the class MappingTable.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class GlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MappingTable
 * @ingroup PEARL_base
 * @brief   Provides functionality for mapping local identifiers of static
 *          program entities to global IDs.
 */
/*-------------------------------------------------------------------------*/

class MappingTable
{
  public:
    /// @name Constructors & destructor
    /// @{

    MappingTable(const GlobalDefs& defs, const std::string& dirname,
                 uint32_t rank);
    ~MappingTable();

    /// @}
    /// @name Mapping routines
    /// @{

    timestamp_t map_time(elg_d8 time) const;

    ident_t map_location(elg_ui4 id) const;
    ident_t map_region(elg_ui4 id) const;
    ident_t map_callsite(elg_ui4 id) const;
    ident_t map_group(elg_ui4 id) const;
    ident_t map_communicator(elg_ui4 id) const;
    ident_t map_window(elg_ui4 id) const;

    /// @}
    /// @name Callback-related methods
    /// @{

    void set_rank(elg_ui4 rank);
    void set_finished();
    void set_time(elg_d8 ltime, elg_d8 offset);
    void set_table(elg_ui1 type, elg_ui1 mode, elg_ui4 count, elg_ui4* data);
    void set_message(const std::string& message);

    /// @}


  private:
    /// Stores the rank number found in the section header
    uint32_t m_rank;

    /// Signals whether the section is entirely read
    bool m_finished;

    /// Counts the number of offset records read
    int m_offcnt;

    /// Local time at beginning and end of execution
    elg_d8 m_ltime[2];

    /// Time offset at beginning and end of execution
    elg_d8 m_offset[2];

    /// Mapping data: local location id |-@> global location id
    EpkIdMap* m_locations;

    /// Mapping data: local region id |-@> global region id
    EpkIdMap* m_regions;

    /// Mapping data: local call site id |-@> global call site id
    EpkIdMap* m_csites;

    /// Mapping data: local MPI group id |-@> global MPI group id
    EpkIdMap* m_groups;

    /// Mapping data: local MPI communicator id |-@> global MPI communicator id
    EpkIdMap* m_communicators;

    /// Mapping data: local RMA window id |-@> global RMA window id
    EpkIdMap* m_windows;

    /// Stores the what() string of exceptions thrown in callbacks,
    /// empty if everything was ok
    std::string m_message;
};


}   /* namespace pearl */


#endif   /* !PEARL_MAPPINGTABLE_H */
