/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>
#include <sys/stat.h>

#include <elg_error.h>
#include <elg_readcb.h>

#include "Error.h"
#include "GlobalDefs.h"
#include "Location.h"
#include "MappingTable.h"
#include "Process.h"
#include "Thread.h"

using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

namespace
{

ident_t find_id(const EpkIdMap* container,
                elg_ui4         id,
                const string&   desc)
{
  if (container == NULL)
    return id;

  ident_t result = epk_idmap_get(container, id);
  if (result == ELG_NO_ID)
    throw RuntimeError("MappingTable::" + desc + " -- Unknown ID.");

  return result;
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class MappingTable
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MappingTable::MappingTable(const GlobalDefs& defs,
                           const string&     dirname,
                           uint32_t          rank)
  : m_rank(PEARL_NO_ID),
    m_offcnt(0),
    m_locations(NULL),
    m_regions(NULL),
    m_csites(NULL),
    m_groups(NULL),
    m_communicators(NULL),
    m_windows(NULL)
{
  // Initialize attributes
  m_ltime[0]  = 0.0;
  m_ltime[1]  = 1.0;
  m_offset[0] = 0.0;
  m_offset[1] = 0.0;

  // Check whether mapping file exists
  string name = dirname + "/epik.map";
  struct stat sbuf; 
  if (stat(name.c_str(), &sbuf) != 0) { 
    elg_cntl_msg("No mapping file found -- Assuming already globalized trace"); 
    return; 
  }

  // Read mapping file
  ElgRCB* file = elg_read_open(name.c_str());

  // Read header
  elg_read_seek(file, defs.get_offset(rank));
  elg_read_next_def(file, this);
  if (m_rank != rank)
    throw FatalError("Found unrelated mapping file section");

  // Read data in section
  m_finished = false;
  while (elg_read_next_def(file, this) && !m_finished)
    ;
  elg_read_close(file);

  // Check for errors
  if (!m_message.empty())
    throw Error(m_message);
}


MappingTable::~MappingTable()
{
  // Release resources
  if (m_locations)
    epk_idmap_free(m_locations);
  if (m_regions)
    epk_idmap_free(m_regions);
  if (m_csites)
    epk_idmap_free(m_csites);
  if (m_groups)
    epk_idmap_free(m_groups);
  if (m_communicators)
    epk_idmap_free(m_communicators);
  if (m_windows)
    epk_idmap_free(m_windows);
}


//--- Mapping routines ------------------------------------------------------

timestamp_t MappingTable::map_time(elg_d8 time) const
{
  return time + (((m_offset[1] - m_offset[0]) / (m_ltime[1] - m_ltime[0])) * 
                 (time - m_ltime[0])) + m_offset[0];
}


ident_t MappingTable::map_location(elg_ui4 id) const
{
  return find_id(m_locations, id, "map_location(elg_ui4)");
}


ident_t MappingTable::map_region(elg_ui4 id) const
{
  return find_id(m_regions, id, "map_region(elg_ui4)");
}


ident_t MappingTable::map_callsite(elg_ui4 id) const
{
  return find_id(m_csites, id, "map_callsite(elg_ui4)");
}


ident_t MappingTable::map_group(elg_ui4 id) const
{
  return find_id(m_groups, id, "map_group(elg_ui4)");
}


ident_t MappingTable::map_communicator(elg_ui4 id) const
{
  return find_id(m_communicators, id, "map_communicator(elg_ui4)");
}


ident_t MappingTable::map_window(elg_ui4 id) const
{
  return find_id(m_windows, id, "map_window(elg_ui4)");
}


//--- Callback-related methods ----------------------------------------------

void MappingTable::set_rank(elg_ui4 rank)
{
  m_rank = rank;
}


void MappingTable::set_finished()
{
  m_finished = true;
}


void MappingTable::set_time(elg_d8 ltime, elg_d8 offset)
{
  if (m_offcnt > 1)
    throw RuntimeError("MappingTable::set_time(elg_d8,elg_d8) -- "
                       "Found more than two ELG_OFFSET records");

  m_ltime[m_offcnt]  = ltime;
  m_offset[m_offcnt] = offset;
  m_offcnt++;
}


void MappingTable::set_table(elg_ui1  type,
                             elg_ui1  mode,
                             elg_ui4  count,
                             elg_ui4* data)
{
  // Determine map
  EpkIdMap** map;
  switch (type) {
    case ELG_LOCATION:
      map = &m_locations;
      break;
    case ELG_REGION:
      map = &m_regions;
      break;
    case ELG_CALL_SITE:
      map = &m_csites;
      break;
    case ELG_MPI_GROUP:
      map = &m_groups;
      break;
    case ELG_MPI_COMM:
      map = &m_communicators;
      break;
    case ELG_MPI_WIN:
      map = &m_windows;
      break;
    case ELG_CALL_PATH:
      // Ignore call-path mapping for backwards compatibility
      return;
    default:
      throw RuntimeError("MappingTable::set_table(...) -- "
                         "Unknown table type");
  }
  if (*map)
    throw RuntimeError("MappingTable::set_table(...) -- Table already defined");

  if (mode == ELG_MAP_DENSE) {
    *map = epk_idmap_create(EPK_IDMAP_DENSE, count);
    for (elg_ui4 index = 0; index < count; ++index)
      epk_idmap_add(*map, index, data[index]);
  } else {   // ELG_MAP_SPARSE
    *map = epk_idmap_create(EPK_IDMAP_SPARSE, count/2);
    for (elg_ui4 index = 0; index < count; index += 2)
      epk_idmap_add(*map, data[index], data[index + 1]);
  }
}


void MappingTable::set_message(const string& message)
{
  m_message = message;
}
